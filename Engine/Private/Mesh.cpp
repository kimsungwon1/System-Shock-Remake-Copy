#include "..\Public\Mesh.h"
#include "GameInstance.h"
#include <numeric>
#undef min;
#undef max;

CMesh::CMesh(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CVIBuffer { pDevice, pContext }
{
}

CMesh::CMesh(const CMesh & Prototype)
	: CVIBuffer{ Prototype }
{
}

HRESULT CMesh::Initialize_Prototype(const CModel* pModel, const aiMesh * pAIMesh, _fmatrix PreTransformMatrix)
{
	strcpy_s(m_szName, pAIMesh->mName.data);
	m_iMaterialIndex = pAIMesh->mMaterialIndex;
	m_iNumVertexBuffers = 1;
	m_iNumVertices = pAIMesh->mNumVertices;
	m_iNumIndices = pAIMesh->mNumFaces * 3;
	m_iIndexStride = 4;
	m_eIndexFormat = DXGI_FORMAT_R32_UINT;
	m_eTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	m_pVerticesPos = new _float3[m_iNumVertices];
	m_triangles = new MeshFace[pAIMesh->mNumFaces];
	m_iNumTriangles = pAIMesh->mNumFaces;

	//memcpy(m_pVerticesPos, pAIMesh->mVertices, sizeof(_float3) * m_iNumVertices);

#pragma region VERTEX_BUFFER

	HRESULT hr = pModel->GetType() == CModel::TYPE_NONANIM ? Ready_VertexBuffer_NonAnim(pAIMesh, PreTransformMatrix) : Ready_VertexBuffer_Anim(pModel, pAIMesh);
	if (FAILED(hr))
		return E_FAIL;

#pragma endregion

#pragma region INDEX_BUFFER
	/* 인덱스버퍼의 내용을 채워주곡 */
	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);
	m_BufferDesc.ByteWidth = m_iIndexStride * m_iNumIndices;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT; /* 정적버퍼로 생성한다. */
	m_BufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = m_iIndexStride;

	_uint* pIndices = new _uint[m_iNumIndices];
	ZeroMemory(pIndices, sizeof(_uint) * m_iNumIndices);

	_uint		iNumIndices = { 0 };

	for (size_t i = 0; i < pAIMesh->mNumFaces; i++)
	{
		aiFace		AIFace = pAIMesh->mFaces[i];

		pIndices[iNumIndices++] = m_triangles[i].iIndices[0] = AIFace.mIndices[0];
		pIndices[iNumIndices++] = m_triangles[i].iIndices[1] = AIFace.mIndices[1];
		pIndices[iNumIndices++] = m_triangles[i].iIndices[2] = AIFace.mIndices[2];
	}

	ZeroMemory(&m_InitialData, sizeof m_InitialData);
	m_InitialData.pSysMem = pIndices;

	/* 인덱스버퍼를 생성한다. */
	if (FAILED(__super::Create_Buffer(&m_pIB)))
		return E_FAIL;

	Safe_Delete_Array(pIndices);

#pragma endregion


	return S_OK;
}

HRESULT CMesh::Initialize(void * pArg)
{
	return S_OK;
}

HRESULT CMesh::Bind_BoneMatrices(const CModel* pModel, CShader* pShader, const char* pConstantName)
{
	ZeroMemory(m_BoneMatrices, sizeof(_float4x4) * g_iMaxMeshBones);

	for (size_t i = 0; i < m_iNumBones; i++)
	{
		XMStoreFloat4x4(&m_BoneMatrices[i], XMLoadFloat4x4(&m_OffsetMatrices[i])
			* pModel->Get_BoneCombinedTransformationMatrix(m_BoneIndices[i]));
	}

	return pShader->Bind_Matrices(pConstantName, m_BoneMatrices, m_iNumBones);
}

HRESULT CMesh::SaveMesh(const CModel* pModel, ofstream& saveStream, const aiMesh* pAIMesh, _fmatrix PreTransformMatrix)
{
	saveStream.write(reinterpret_cast<const char*>(&m_iMaterialIndex), sizeof(m_iMaterialIndex));
	saveStream.write(reinterpret_cast<const char*>(&m_iNumVertices), sizeof(m_iNumVertices));
	saveStream.write(reinterpret_cast<const char*>(&m_iNumIndices), sizeof(m_iNumIndices));

	if (pModel->GetType() == CModel::TYPE_NONANIM)
	{
		saveStream.write(reinterpret_cast<const char*>(&m_iVertexStride), sizeof(m_iVertexStride));
		VTXMESH* pVertices = new VTXMESH[m_iNumVertices];
		ZeroMemory(pVertices, sizeof(VTXMESH) * m_iNumVertices);

		for (size_t i = 0; i < m_iNumVertices; i++)
		{
			memcpy(&pVertices[i].vPosition, &pAIMesh->mVertices[i], sizeof(_float3));
			XMStoreFloat3(&pVertices[i].vPosition, XMVector3TransformCoord(XMLoadFloat3(&pVertices[i].vPosition), PreTransformMatrix));

			memcpy(&pVertices[i].vNormal, &pAIMesh->mNormals[i], sizeof(_float3));
			XMStoreFloat3(&pVertices[i].vNormal, XMVector3TransformCoord(XMLoadFloat3(&pVertices[i].vNormal), PreTransformMatrix));

			memcpy(&pVertices[i].vTexcoord, &pAIMesh->mTextureCoords[0][i], sizeof(_float2));
			memcpy(&pVertices[i].vTangent, &pAIMesh->mTangents[i], sizeof(_float3));
			XMStoreFloat3(&pVertices[i].vTangent, XMVector3TransformCoord(XMLoadFloat3(&pVertices[i].vTangent), PreTransformMatrix));
		}

		saveStream.write(reinterpret_cast<const char*>(pVertices), (sizeof(VTXMESH) * m_iNumVertices));

		Safe_Delete_Array(pVertices);
	}
	else 
	{
		saveStream.write(reinterpret_cast<const char*>(&m_iVertexStride), sizeof(m_iVertexStride));
		VTXANIMMESH* pVertices = new VTXANIMMESH[m_iNumVertices];
		ZeroMemory(pVertices, sizeof(VTXANIMMESH) * m_iNumVertices);
		
		saveStream.write(reinterpret_cast<const char*>(&m_iNumBones), sizeof(m_iNumBones));

		for (size_t i = 0; i < m_iNumBones; i++)
		{
			saveStream.write(reinterpret_cast<const char*>(&m_OffsetMatrices[i]), sizeof(_float4x4));
			saveStream.write(reinterpret_cast<const char*>(&m_BoneIndices[i]), sizeof(_int));
		}

		for (size_t i = 0; i < m_iNumVertices; i++)
		{
			memcpy(&pVertices[i].vPosition, &pAIMesh->mVertices[i], sizeof(_float3));
			memcpy(&pVertices[i].vNormal, &pAIMesh->mNormals[i], sizeof(_float3));
			memcpy(&pVertices[i].vTexcoord, &pAIMesh->mTextureCoords[0][i], sizeof(_float2));
			memcpy(&pVertices[i].vTangent, &pAIMesh->mTangents[i], sizeof(_float3));
		}

		for (size_t i = 0; i < m_iNumBones; i++)
		{
			aiBone* pAIBone = pAIMesh->mBones[i];
			/* 이 뼈는 몇개(mNumWeights) 정점에게 영향을 주는가? */
			for (size_t j = 0; j < pAIBone->mNumWeights; j++)
			{
				/* 이 뼈가 영향을 주는 j번째 정점의 인덱스(pAIBone->mWeights[j].mVertexId)*/
				if (0.f == pVertices[pAIBone->mWeights[j].mVertexId].vBlendWeights.x)
				{
					pVertices[pAIBone->mWeights[j].mVertexId].vBlendIndices.x = i;
					pVertices[pAIBone->mWeights[j].mVertexId].vBlendWeights.x = pAIBone->mWeights[j].mWeight;
				}

				else if (0.f == pVertices[pAIBone->mWeights[j].mVertexId].vBlendWeights.y)
				{
					pVertices[pAIBone->mWeights[j].mVertexId].vBlendIndices.y = i;
					pVertices[pAIBone->mWeights[j].mVertexId].vBlendWeights.y = pAIBone->mWeights[j].mWeight;
				}

				else if (0.f == pVertices[pAIBone->mWeights[j].mVertexId].vBlendWeights.z)
				{
					pVertices[pAIBone->mWeights[j].mVertexId].vBlendIndices.z = i;
					pVertices[pAIBone->mWeights[j].mVertexId].vBlendWeights.z = pAIBone->mWeights[j].mWeight;
				}

				else
				{
					pVertices[pAIBone->mWeights[j].mVertexId].vBlendIndices.w = i;
					pVertices[pAIBone->mWeights[j].mVertexId].vBlendWeights.w = pAIBone->mWeights[j].mWeight;
				}
			}
		}
		saveStream.write(reinterpret_cast<const char*>(pVertices), sizeof(VTXANIMMESH) * m_iNumVertices);
		Safe_Delete_Array(pVertices);

	}

	_uint* pIndices = new _uint[m_iNumIndices];
	ZeroMemory(pIndices, sizeof(_uint) * m_iNumIndices);

	_uint		iNumIndices = { 0 };

	for (size_t i = 0; i < pAIMesh->mNumFaces; i++)
	{
		aiFace		AIFace = pAIMesh->mFaces[i];

		pIndices[iNumIndices++] = AIFace.mIndices[0];
		pIndices[iNumIndices++] = AIFace.mIndices[1];
		pIndices[iNumIndices++] = AIFace.mIndices[2];
	}
	saveStream.write(reinterpret_cast<const char*>(pIndices), (sizeof(_uint) * m_iNumIndices));

	Safe_Delete_Array(pIndices);

	return S_OK;
}

HRESULT CMesh::LoadMesh(const CModel* pModel, ifstream& loadStream)
{
	loadStream.read(reinterpret_cast<char*>(&m_iMaterialIndex), sizeof(m_iMaterialIndex));
	loadStream.read(reinterpret_cast<char*>(&m_iNumVertices), sizeof(m_iNumVertices));
	loadStream.read(reinterpret_cast<char*>(&m_iNumIndices), sizeof(m_iNumIndices));
	m_iNumVertexBuffers = 1;
	m_iIndexStride = 4;
	m_eIndexFormat = DXGI_FORMAT_R32_UINT;
	m_eTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	m_pVerticesPos = new _float3[m_iNumVertices];


	if (pModel->GetType() == CModel::TYPE_NONANIM)
	{
		_float minX = numeric_limits<_float>::max(); _float maxX = numeric_limits<_float>::min();
		_float minY = numeric_limits<_float>::max(); _float maxY = numeric_limits<_float>::min();
		_float minZ = numeric_limits<_float>::max(); _float maxZ = numeric_limits<_float>::min();

		loadStream.read(reinterpret_cast<char*>(&m_iVertexStride), sizeof(m_iVertexStride));

		ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);
		m_BufferDesc.ByteWidth = m_iVertexStride * m_iNumVertices;
		m_BufferDesc.Usage = D3D11_USAGE_DEFAULT; /* 정적버퍼로 생성한다. */
		m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		m_BufferDesc.CPUAccessFlags = 0;
		m_BufferDesc.MiscFlags = 0;
		m_BufferDesc.StructureByteStride = m_iVertexStride;
		
		VTXMESH* pVertices = new VTXMESH[m_iNumVertices];
		ZeroMemory(pVertices, sizeof(VTXMESH) * m_iNumVertices);

		loadStream.read(reinterpret_cast<char*>(pVertices), sizeof(VTXMESH) * m_iNumVertices);

		for (int i = 0; i < m_iNumVertices; i++) {
			if (pVertices[i].vPosition.x < minX) {
				minX = pVertices[i].vPosition.x;
			}
			if (pVertices[i].vPosition.x > maxX) {
				maxX = pVertices[i].vPosition.x;
			}
			if (pVertices[i].vPosition.y < minY) {
				minY = pVertices[i].vPosition.y;
			}
			if (pVertices[i].vPosition.y > maxY) {
				maxY = pVertices[i].vPosition.y;
			}
			if (pVertices[i].vPosition.z < minZ) {
				minZ = pVertices[i].vPosition.z;
			}
			if (pVertices[i].vPosition.z > maxZ) {
				maxZ = pVertices[i].vPosition.z;
			}

			memcpy(&m_pVerticesPos[i], &pVertices[i].vPosition, sizeof(_float3));
		}
		m_fMaxX = maxX; m_fMinX = minX;
		m_fMaxY = maxY; m_fMinY = minY;
		m_fMaxZ = maxZ; m_fMinZ = minZ;

		ZeroMemory(&m_InitialData, sizeof(m_InitialData));
		m_InitialData.pSysMem = pVertices;

		if (FAILED(__super::Create_Buffer(&m_pVB)))
			return E_FAIL;

		Safe_Delete_Array(pVertices);
	}
	else
	{
		loadStream.read(reinterpret_cast<char*>(&m_iVertexStride), sizeof(m_iVertexStride));
		ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);
		m_BufferDesc.ByteWidth = m_iVertexStride * m_iNumVertices;
		m_BufferDesc.Usage = D3D11_USAGE_DEFAULT; /* 정적버퍼로 생성한다. */
		m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		m_BufferDesc.CPUAccessFlags = 0;
		m_BufferDesc.MiscFlags = 0;
		m_BufferDesc.StructureByteStride = m_iVertexStride;

		loadStream.read(reinterpret_cast<char*>(&m_iNumBones), sizeof(m_iNumBones));
		for (size_t i = 0; i < m_iNumBones; i++)
		{
			_float4x4 offsetMatrix = {};
			_int	iBoneIndex = {};
			loadStream.read(reinterpret_cast<char*>(&offsetMatrix), sizeof(_float4x4));
			loadStream.read(reinterpret_cast<char*>(&iBoneIndex), sizeof(_int));

			m_OffsetMatrices.push_back(offsetMatrix);
			m_BoneIndices.push_back(iBoneIndex);
		}

		VTXANIMMESH* pVertices = new VTXANIMMESH[m_iNumVertices];
		ZeroMemory(pVertices, sizeof(VTXANIMMESH) * m_iNumVertices);

		loadStream.read(reinterpret_cast<char*>(pVertices), sizeof(VTXANIMMESH) * m_iNumVertices);

		for (size_t i = 0; i < m_iNumVertices; i++)
		{
			memcpy(&m_pVerticesPos[i], &pVertices[i].vPosition, sizeof(_float3));
		}

		ZeroMemory(&m_InitialData, sizeof(m_InitialData));
		m_InitialData.pSysMem = pVertices;

		//정점버퍼를 생성한다.
		if (FAILED(__super::Create_Buffer(&m_pVB)))
			return E_FAIL;

		Safe_Delete_Array(pVertices);
	}

	// INDEX BUFFER

	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);
	m_BufferDesc.ByteWidth = m_iIndexStride * m_iNumIndices;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT; /* 정적버퍼로 생성한다. */
	m_BufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = m_iIndexStride;

	_uint* pIndices = new _uint[m_iNumIndices];
	ZeroMemory(pIndices, sizeof(_uint) * m_iNumIndices);

	_uint		iNumIndices = { 0 };

	loadStream.read(reinterpret_cast<char*>(pIndices), sizeof(_uint) * m_iNumIndices);

	ZeroMemory(&m_InitialData, sizeof m_InitialData);
	m_InitialData.pSysMem = pIndices;

	/* 인덱스버퍼를 생성한다. */
	if (FAILED(__super::Create_Buffer(&m_pIB)))
		return E_FAIL;

	Safe_Delete_Array(pIndices);
	return S_OK;
}

HRESULT CMesh::Ready_VertexBuffer_NonAnim(const aiMesh* pAIMesh, _fmatrix PreTransformMatrix)
{
	m_iVertexStride = sizeof(VTXMESH);

	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);
	m_BufferDesc.ByteWidth = m_iVertexStride * m_iNumVertices;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT; /* 정적버퍼로 생성한다. */
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = m_iVertexStride;

	
	/* 정점버퍼에 채워줄 값들을 만들기위해서 임시적으로 공간을 할당한다. */
	VTXMESH* pVertices = new VTXMESH[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTXMESH) * m_iNumVertices);

	_float minX = numeric_limits<_float>::max(); _float maxX = numeric_limits<_float>::min();
	_float minY = numeric_limits<_float>::max(); _float maxY = numeric_limits<_float>::min();
	_float minZ = numeric_limits<_float>::max(); _float maxZ = numeric_limits<_float>::min();

	for (size_t i = 0; i < m_iNumVertices; i++)
	{
		memcpy(&pVertices[i].vPosition, &pAIMesh->mVertices[i], sizeof(_float3));

		XMStoreFloat3(&pVertices[i].vPosition, XMVector3TransformCoord(XMLoadFloat3(&pVertices[i].vPosition), PreTransformMatrix));

		if (pVertices[i].vPosition.x < minX) {
			minX = pVertices[i].vPosition.x;
		}
		if (pVertices[i].vPosition.x > maxX) {
			maxX = pVertices[i].vPosition.x;
		}
		if (pVertices[i].vPosition.y < minY) {
			minY = pVertices[i].vPosition.y;
		}
		if (pVertices[i].vPosition.y > maxY) {
			maxY = pVertices[i].vPosition.y;
		}
		if (pVertices[i].vPosition.z < minZ) {
			minZ = pVertices[i].vPosition.z;
		}
		if (pVertices[i].vPosition.z > maxZ) {
			maxZ = pVertices[i].vPosition.z;
		}
		memcpy(&m_pVerticesPos[i], &pVertices[i].vPosition, sizeof(_float3));

		memcpy(&pVertices[i].vNormal, &pAIMesh->mNormals[i], sizeof(_float3));
		XMStoreFloat3(&pVertices[i].vNormal, XMVector3TransformCoord(XMLoadFloat3(&pVertices[i].vNormal), PreTransformMatrix));
		
		memcpy(&pVertices[i].vTexcoord, &pAIMesh->mTextureCoords[0][i], sizeof(_float2));
		memcpy(&pVertices[i].vTangent, &pAIMesh->mTangents[i], sizeof(_float3));
		XMStoreFloat3(&pVertices[i].vTangent, XMVector3TransformCoord(XMLoadFloat3(&pVertices[i].vTangent), PreTransformMatrix));
	}
	m_fMaxX = maxX; m_fMinX = minX;
	m_fMaxY = maxY; m_fMinY = minY;
	m_fMaxZ = maxZ; m_fMinZ = minZ;

	ZeroMemory(&m_InitialData, sizeof m_InitialData);
	m_InitialData.pSysMem = pVertices;

	/* 정점버퍼를 생성한다. */
	if (FAILED(__super::Create_Buffer(&m_pVB)))
		return E_FAIL;

	Safe_Delete_Array(pVertices);

	return S_OK;
}

HRESULT CMesh::Ready_VertexBuffer_Anim(const CModel* pModel, const aiMesh* pAIMesh)
{
	m_iVertexStride = sizeof(VTXANIMMESH);

	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);
	m_BufferDesc.ByteWidth = m_iVertexStride * m_iNumVertices;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT; /* 정적버퍼로 생성한다. */
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = m_iVertexStride;

	/* 정점버퍼에 채워줄 값들을 만들기위해서 임시적으로 공간을 할당한다. */
	VTXANIMMESH* pVertices = new VTXANIMMESH[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTXANIMMESH) * m_iNumVertices);

	_matrix preMat = pModel->GetPreTransformMatrix();
	_float minX = numeric_limits<_float>::max(); _float maxX = numeric_limits<_float>::min();
	_float minY = numeric_limits<_float>::max(); _float maxY = numeric_limits<_float>::min();
	_float minZ = numeric_limits<_float>::max(); _float maxZ = numeric_limits<_float>::min();

	for (size_t i = 0; i < m_iNumVertices; i++)
	{
		memcpy(&pVertices[i].vPosition, &pAIMesh->mVertices[i], sizeof(_float3));
		
		memcpy(&m_pVerticesPos[i], &pVertices[i].vPosition, sizeof(_float3));

		_vector formedPos = XMVector3TransformCoord(XMLoadFloat3(&pVertices[i].vPosition), preMat);

		if (formedPos.m128_f32[0] < minX) {
			minX = formedPos.m128_f32[0];
		}
		if (formedPos.m128_f32[0] > maxX) {
			maxX = formedPos.m128_f32[0];
		}
		if (formedPos.m128_f32[1] < minY) {
			minY = formedPos.m128_f32[1];
		}
		if (formedPos.m128_f32[1] > maxY) {
			maxY = formedPos.m128_f32[1];
		}
		if (formedPos.m128_f32[2] < minZ) {
			minZ = formedPos.m128_f32[2];
		}
		if (formedPos.m128_f32[2] > maxZ) {
			maxZ = formedPos.m128_f32[2];
		}
		memcpy(&pVertices[i].vNormal, &pAIMesh->mNormals[i], sizeof(_float3));
		memcpy(&pVertices[i].vTexcoord, &pAIMesh->mTextureCoords[0][i], sizeof(_float2));
		memcpy(&pVertices[i].vTangent, &pAIMesh->mTangents[i], sizeof(_float3));
	}
	m_fMaxX = maxX; m_fMinX = minX;
	m_fMaxY = maxY; m_fMinY = minY;
	m_fMaxZ = maxZ; m_fMinZ = minZ;

	/* 모델 전체의 뼈가 아닌 이 메시하나에 영향을 주는 뼈의 갯수. */
	m_iNumBones = pAIMesh->mNumBones;

	m_OffsetMatrices.resize(m_iNumBones);

	for (size_t i = 0; i < m_iNumBones; i++)
	{
		aiBone* pAIBone = pAIMesh->mBones[i];
		
		/* pAIBone->mOffsetMatrix : 여러개의 모델이 하나의 뼈대를 공유하는 경우, 모델의 형태에 따라서 스키닝(정점의 위치 * 뼈행렬)작업 시, 추가적인 보정이 필요할 수 있다.  */
		memcpy(&m_OffsetMatrices[i], &pAIBone->mOffsetMatrix, sizeof(_float4x4));
		XMStoreFloat4x4(&m_OffsetMatrices[i], XMMatrixTranspose(XMLoadFloat4x4(&m_OffsetMatrices[i])));

		/* 내 모델이 들고 있는 전체 뼈들중에 이 메시가 사용하는 뼈와이름이 같은 뼈의 인덱스를 저장해둔다. */
		m_BoneIndices.emplace_back(pModel->Get_BoneIndex(pAIBone->mName.data));
		
		//pAIBone->mName

		/* 이 뼈는 몇개(mNumWeights) 정점에게 영향을 주는가? */
		for (size_t j = 0; j < pAIBone->mNumWeights; j++)
		{
			/* 이 뼈가 영향을 주는 j번째 정점의 인덱스(pAIBone->mWeights[j].mVertexId)*/
			if (0.f == pVertices[pAIBone->mWeights[j].mVertexId].vBlendWeights.x)
			{
				pVertices[pAIBone->mWeights[j].mVertexId].vBlendIndices.x = i;
				pVertices[pAIBone->mWeights[j].mVertexId].vBlendWeights.x = pAIBone->mWeights[j].mWeight;
			}

			else if (0.f == pVertices[pAIBone->mWeights[j].mVertexId].vBlendWeights.y)
			{
				pVertices[pAIBone->mWeights[j].mVertexId].vBlendIndices.y = i;
				pVertices[pAIBone->mWeights[j].mVertexId].vBlendWeights.y = pAIBone->mWeights[j].mWeight;
			}

			else if (0.f == pVertices[pAIBone->mWeights[j].mVertexId].vBlendWeights.z)
			{
				pVertices[pAIBone->mWeights[j].mVertexId].vBlendIndices.z = i;
				pVertices[pAIBone->mWeights[j].mVertexId].vBlendWeights.z = pAIBone->mWeights[j].mWeight;
			}

			else
			{
				pVertices[pAIBone->mWeights[j].mVertexId].vBlendIndices.w = i;
				pVertices[pAIBone->mWeights[j].mVertexId].vBlendWeights.w = pAIBone->mWeights[j].mWeight;
			}
		}
	}

	if (0 == m_iNumBones)
	{
		m_iNumBones = 1;

		m_BoneIndices.emplace_back(pModel->Get_BoneIndex(m_szName));

		_float4x4 OffsetMatrix;
		XMStoreFloat4x4(&OffsetMatrix, XMMatrixIdentity());
		m_OffsetMatrices.emplace_back(OffsetMatrix);
	}

	ZeroMemory(&m_InitialData, sizeof m_InitialData);
	m_InitialData.pSysMem = pVertices;

	/* 정점버퍼를 생성한다. */
	if (FAILED(__super::Create_Buffer(&m_pVB)))
		return E_FAIL;

	Safe_Delete_Array(pVertices);

	return S_OK;
}

_bool CMesh::isPicking(const _float4x4& WorldMatrix, _float3* pOut)
{
	m_pGameInstance->Transform_MouseRay_ToLocalSpace(WorldMatrix);
	/*for (size_t i = 0; i < m_iNumTriangles; i++)
	{
		if (m_pGameInstance->isPicked_InLocalSpace(
			XMLoadFloat3(&m_pVerticesPos[m_triangles[i].iIndices[0]]),
			XMLoadFloat3(&m_pVerticesPos[m_triangles[i].iIndices[1]]),
			XMLoadFloat3(&m_pVerticesPos[m_triangles[i].iIndices[2]]),
			pOut)) 
		{
			XMStoreFloat3(pOut, XMVector3TransformCoord(XMLoadFloat3(pOut), XMLoadFloat4x4(&WorldMatrix)));
			return true;
		}
	}
	return false;*/
	//1
	if (m_pGameInstance->isPicked_InLocalSpace(
		XMVectorSet(m_fMinX, m_fMaxY, m_fMinZ, 0.f),
		XMVectorSet(m_fMaxX, m_fMaxY, m_fMinZ, 0.f),
		XMVectorSet(m_fMaxX, m_fMinY, m_fMinZ, 0.f),
		pOut
	)) {
		goto Compute_WorldPos;
	}
	if (m_pGameInstance->isPicked_InLocalSpace(
		XMVectorSet(m_fMinX, m_fMaxY, m_fMinZ, 0.f),
		XMVectorSet(m_fMaxX, m_fMinY, m_fMinZ, 0.f),
		XMVectorSet(m_fMinX, m_fMinY, m_fMinZ, 0.f),
		pOut
	)) {
		goto Compute_WorldPos;
	}
	//2
	if (m_pGameInstance->isPicked_InLocalSpace(
		XMVectorSet(m_fMinX, m_fMaxY, m_fMaxZ, 0.f),
		XMVectorSet(m_fMinX, m_fMaxY, m_fMinZ, 0.f),
		XMVectorSet(m_fMinX, m_fMinY, m_fMinZ, 0.f),
		pOut
	)) {
		goto Compute_WorldPos;
	}
	if (m_pGameInstance->isPicked_InLocalSpace(
		XMVectorSet(m_fMinX, m_fMaxY, m_fMaxZ, 0.f),
		XMVectorSet(m_fMinX, m_fMinY, m_fMinZ, 0.f),
		XMVectorSet(m_fMinX, m_fMinY, m_fMaxZ, 0.f),
		pOut
	)) {
		goto Compute_WorldPos;
	}
	//3
	if (m_pGameInstance->isPicked_InLocalSpace(
		XMVectorSet(m_fMinX, m_fMaxY, m_fMaxZ, 0.f),
		XMVectorSet(m_fMaxX, m_fMaxY, m_fMaxZ, 0.f),
		XMVectorSet(m_fMaxX, m_fMaxY, m_fMinZ, 0.f),
		pOut
	)) {
		goto Compute_WorldPos;
	}
	if (m_pGameInstance->isPicked_InLocalSpace(
		XMVectorSet(m_fMinX, m_fMaxY, m_fMaxZ, 0.f),
		XMVectorSet(m_fMaxX, m_fMaxY, m_fMinZ, 0.f),
		XMVectorSet(m_fMinX, m_fMaxY, m_fMinZ, 0.f),
		pOut
	)) {
		goto Compute_WorldPos;
	}
	//4
	if (m_pGameInstance->isPicked_InLocalSpace(
		XMVectorSet(m_fMinX, m_fMinY, m_fMinZ, 0.f),
		XMVectorSet(m_fMaxX, m_fMinY, m_fMinZ, 0.f),
		XMVectorSet(m_fMaxX, m_fMinY, m_fMaxZ, 0.f),
		pOut
	)) {
		goto Compute_WorldPos;
	}
	if (m_pGameInstance->isPicked_InLocalSpace(
		XMVectorSet(m_fMinX, m_fMinY, m_fMinZ, 0.f),
		XMVectorSet(m_fMaxX, m_fMinY, m_fMaxZ, 0.f),
		XMVectorSet(m_fMinX, m_fMinY, m_fMaxZ, 0.f),
		pOut
	)) {
		goto Compute_WorldPos;
	}
	//5
	if (m_pGameInstance->isPicked_InLocalSpace(
		XMVectorSet(m_fMaxX, m_fMaxY, m_fMinZ, 0.f),
		XMVectorSet(m_fMaxX, m_fMaxY, m_fMaxZ, 0.f),
		XMVectorSet(m_fMaxX, m_fMinY, m_fMaxZ, 0.f),
		pOut
	)) {
		goto Compute_WorldPos;
	}
	if (m_pGameInstance->isPicked_InLocalSpace(
		XMVectorSet(m_fMaxX, m_fMaxY, m_fMinZ, 0.f),
		XMVectorSet(m_fMaxX, m_fMinY, m_fMaxZ, 0.f),
		XMVectorSet(m_fMaxX, m_fMinY, m_fMinZ, 0.f),
		pOut
	)) {
		goto Compute_WorldPos;
	}
	//6
	if (m_pGameInstance->isPicked_InLocalSpace(
		XMVectorSet(m_fMaxX, m_fMaxY, m_fMaxZ, 0.f),
		XMVectorSet(m_fMinX, m_fMaxY, m_fMaxZ, 0.f),
		XMVectorSet(m_fMinX, m_fMinY, m_fMaxZ, 0.f),
		pOut
	)) {
		goto Compute_WorldPos;
	}
	if (m_pGameInstance->isPicked_InLocalSpace(
		XMVectorSet(m_fMaxX, m_fMaxY, m_fMaxZ, 0.f),
		XMVectorSet(m_fMinX, m_fMinY, m_fMaxZ, 0.f),
		XMVectorSet(m_fMaxX, m_fMinY, m_fMaxZ, 0.f),
		pOut
	)) {
		goto Compute_WorldPos;
	}
	return false;
Compute_WorldPos:
	XMStoreFloat3(pOut, XMVector3TransformCoord(XMLoadFloat3(pOut), XMLoadFloat4x4(&WorldMatrix)));
	return true;
}

_bool CMesh::LineIntersects(_fvector vWorldRay_Pos, _fvector vWorldRay_Dir, _cmatrix worldMatrix, _float* pfDistance)
{
	_bool isIntersect = false;
	if (lineIntersects_perTriangle(
		XMVectorSet(m_fMinX, m_fMaxY, m_fMinZ, 1.f),
		XMVectorSet(m_fMaxX, m_fMaxY, m_fMinZ, 1.f),
		XMVectorSet(m_fMaxX, m_fMinY, m_fMinZ, 1.f), vWorldRay_Pos, vWorldRay_Dir, worldMatrix, pfDistance
	)) {
		isIntersect = true;
	}
	else if (lineIntersects_perTriangle(
		XMVectorSet(m_fMinX, m_fMaxY, m_fMinZ, 1.f),
		XMVectorSet(m_fMaxX, m_fMinY, m_fMinZ, 1.f),
		XMVectorSet(m_fMinX, m_fMinY, m_fMinZ, 1.f), vWorldRay_Pos, vWorldRay_Dir, worldMatrix, pfDistance
	)) {
		isIntersect = true;
	}
	//2
	else if (lineIntersects_perTriangle(
		XMVectorSet(m_fMinX, m_fMaxY, m_fMaxZ, 1.f),
		XMVectorSet(m_fMinX, m_fMaxY, m_fMinZ, 1.f),
		XMVectorSet(m_fMinX, m_fMinY, m_fMinZ, 1.f), vWorldRay_Pos, vWorldRay_Dir, worldMatrix, pfDistance
	)) {
		isIntersect = true;
	}
	else if (lineIntersects_perTriangle(
		XMVectorSet(m_fMinX, m_fMaxY, m_fMaxZ, 1.f),
		XMVectorSet(m_fMinX, m_fMinY, m_fMinZ, 1.f),
		XMVectorSet(m_fMinX, m_fMinY, m_fMaxZ, 1.f), vWorldRay_Pos, vWorldRay_Dir, worldMatrix, pfDistance
	)) {
		isIntersect = true;
	}
	//3
	else if (lineIntersects_perTriangle(
		XMVectorSet(m_fMinX, m_fMaxY, m_fMaxZ, 1.f),
		XMVectorSet(m_fMaxX, m_fMaxY, m_fMaxZ, 1.f),
		XMVectorSet(m_fMaxX, m_fMaxY, m_fMinZ, 1.f), vWorldRay_Pos, vWorldRay_Dir, worldMatrix, pfDistance
	)) {
		isIntersect = true;
	}
	else if (lineIntersects_perTriangle(
		XMVectorSet(m_fMinX, m_fMaxY, m_fMaxZ, 1.f),
		XMVectorSet(m_fMaxX, m_fMaxY, m_fMinZ, 1.f),
		XMVectorSet(m_fMinX, m_fMaxY, m_fMinZ, 1.f), vWorldRay_Pos, vWorldRay_Dir, worldMatrix, pfDistance
	)) {
		isIntersect = true;
	}
	//4
	else if (lineIntersects_perTriangle(
		XMVectorSet(m_fMinX, m_fMinY, m_fMinZ, 1.f),
		XMVectorSet(m_fMaxX, m_fMinY, m_fMinZ, 1.f),
		XMVectorSet(m_fMaxX, m_fMinY, m_fMaxZ, 1.f), vWorldRay_Pos, vWorldRay_Dir, worldMatrix, pfDistance
	)) {
		isIntersect = true;
	}
	else if (lineIntersects_perTriangle(
		XMVectorSet(m_fMinX, m_fMinY, m_fMinZ, 1.f),
		XMVectorSet(m_fMaxX, m_fMinY, m_fMaxZ, 1.f),
		XMVectorSet(m_fMinX, m_fMinY, m_fMaxZ, 1.f), vWorldRay_Pos, vWorldRay_Dir, worldMatrix, pfDistance
	)) {
		isIntersect = true;
	}
	//5
	else if (lineIntersects_perTriangle(
		XMVectorSet(m_fMaxX, m_fMaxY, m_fMinZ, 1.f),
		XMVectorSet(m_fMaxX, m_fMaxY, m_fMaxZ, 1.f),
		XMVectorSet(m_fMaxX, m_fMinY, m_fMaxZ, 1.f), vWorldRay_Pos, vWorldRay_Dir, worldMatrix, pfDistance
	)) {
		isIntersect = true;
	}
	else if (lineIntersects_perTriangle(
		XMVectorSet(m_fMaxX, m_fMaxY, m_fMinZ, 1.f),
		XMVectorSet(m_fMaxX, m_fMinY, m_fMaxZ, 1.f),
		XMVectorSet(m_fMaxX, m_fMinY, m_fMinZ, 1.f), vWorldRay_Pos, vWorldRay_Dir, worldMatrix, pfDistance
	)) {
		isIntersect = true;
	}
	//6
	else if (lineIntersects_perTriangle(
		XMVectorSet(m_fMaxX, m_fMaxY, m_fMaxZ, 1.f),
		XMVectorSet(m_fMinX, m_fMaxY, m_fMaxZ, 1.f),
		XMVectorSet(m_fMinX, m_fMinY, m_fMaxZ, 1.f), vWorldRay_Pos, vWorldRay_Dir, worldMatrix, pfDistance
	)) {
		isIntersect = true;
	}
	else if (lineIntersects_perTriangle(
		XMVectorSet(m_fMaxX, m_fMaxY, m_fMaxZ, 1.f),
		XMVectorSet(m_fMinX, m_fMinY, m_fMaxZ, 1.f),
		XMVectorSet(m_fMaxX, m_fMinY, m_fMaxZ, 1.f), vWorldRay_Pos, vWorldRay_Dir, worldMatrix, pfDistance
	)) {
		isIntersect = true;
	}

	return isIntersect;
}

_bool CMesh::lineIntersects_perTriangle(_fvector vPointA, _fvector vPointB, _fvector vPointC, _gvector vWorldRay_Pos, _hvector vWorldRay_Dir, _cmatrix worldMatrix, _float* pfDistance)
{
	_vector a = XMVector3TransformCoord(vPointA, worldMatrix);
	_vector b = XMVector3TransformCoord(vPointB, worldMatrix);
	_vector c = XMVector3TransformCoord(vPointC, worldMatrix);

	_bool bIntersects = TriangleTests::Intersects(vWorldRay_Pos, XMVector3Normalize(vWorldRay_Dir), a, b, c, *pfDistance);
	return bIntersects;
}

CMesh * CMesh::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const CModel* pModel, const aiMesh * pAIMesh, _fmatrix PreTransformMatrix)
{
	CMesh*		pInstance = new CMesh(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(pModel, pAIMesh, PreTransformMatrix)))
	{
		MSG_BOX(TEXT("Failed to Created : CMesh"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CMesh* CMesh::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CModel* pModel, ifstream& loadStream)
{
	CMesh* pInstance = new CMesh(pDevice, pContext);

	if (FAILED(pInstance->LoadMesh(pModel, loadStream))) {
		MSG_BOX(TEXT("Failed to Created : CMesh"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



CComponent * CMesh::Clone(void * pArg)
{
	CMesh*		pInstance = new CMesh(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CMesh"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMesh::Free()
{
	__super::Free();
	Safe_Delete_Array(m_triangles);
}
