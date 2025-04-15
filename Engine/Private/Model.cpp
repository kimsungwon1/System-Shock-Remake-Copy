#include "..\Public\Model.h"
#include "Mesh.h"
#include "Texture.h"
#include "Bone.h"
#include "Animation.h"

#include <numeric>
#undef min;
#undef max;

double CModel::s_dAnimToAnimSpeed = 3.0;

CModel::CModel(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CComponent { pDevice, pContext }
	, m_iCurrentAnimIndex{ numeric_limits<_uint>::max() }
	, m_iNextAnimIndex { numeric_limits<_uint>::max() }
{

}

CModel::CModel(const CModel & Prototype)
	: CComponent{ Prototype }
	, m_iNumMeshes { Prototype.m_iNumMeshes } 
	, m_Meshes{ Prototype.m_Meshes }
	, m_iNumMaterials{ Prototype.m_iNumMaterials }
	, m_Materials { Prototype.m_Materials }
	, m_PreTransformMatrix { Prototype.m_PreTransformMatrix }
	, m_iCurrentAnimIndex { Prototype.m_iCurrentAnimIndex }
	, m_iNextAnimIndex { Prototype.m_iNextAnimIndex }
	, m_iNumAnimations { Prototype.m_iNumAnimations }
	, m_eType { Prototype.m_eType }
{
	for (auto& pPrototypeAnimation : Prototype.m_Animations)
		m_Animations.emplace_back(pPrototypeAnimation->Clone());

	for (auto& pPrototypeBone : Prototype.m_Bones)
		m_Bones.emplace_back(pPrototypeBone->Clone());

	for (auto& Material : m_Materials)
	{
		for (auto& pTexture : Material.pMaterialTextures)
			Safe_AddRef(pTexture);
	}
		

	for (auto& pMesh : m_Meshes)	
		Safe_AddRef(pMesh);
	

}

_int CModel::Get_BoneIndex(const _char* pBoneName) const
{
	_uint iBoneIndex = { 0 };
	auto iter = find_if(m_Bones.begin(), m_Bones.end(), [&](CBone* pBone)->_bool 
		{
			if (0 == strcmp(pBone->Get_Name(), pBoneName))
				return true;
			++iBoneIndex;
			return false;
		});
	if (iter == m_Bones.end())
		MSG_BOX(TEXT("없음!"));

	return iBoneIndex;
}

const _float4x4* CModel::Get_BoneCombinedTransformMatrix_Ptr(const _char* pConstantName) const
{
	return m_Bones[this->Get_BoneIndex(pConstantName)]->Get_CombinedTransformMatrix_Ptr();
}

HRESULT CModel::Initialize_Prototype(TYPE eType, const _char * pModelFilePath, _fmatrix PreTransformMatrix)
{
	_uint		iFlag = { 0 };
	
	/* 이전 : 모든 메시가 다 원점을 기준으로 그렺니다. */
	/* 이후 : 모델을 구성하는 모든 메시들을 각각 정해진 상태(메시를 배치하기위한 뼈대의 위치에 맞춰서)대로 미리 변환해준다.*/

	iFlag = aiProcess_ConvertToLeftHanded | aiProcessPreset_TargetRealtime_Fast;	

	/*aiProcess_GlobalScale*/

	if (eType == TYPE_NONANIM)
		iFlag |= aiProcess_PreTransformVertices;	

	m_pAIScene = m_Importer.ReadFile(pModelFilePath, iFlag);
	if (nullptr == m_pAIScene)
		return E_FAIL;

	XMStoreFloat4x4(&m_PreTransformMatrix, PreTransformMatrix);
	m_eType = eType;

	if (FAILED(Ready_Bones(m_pAIScene->mRootNode, -1)))
		return E_FAIL;

	if (FAILED(Ready_Meshes()))
		return E_FAIL;	

	if (FAILED(Ready_Materials(pModelFilePath)))
		return E_FAIL;

	if (FAILED(Ready_Animations()))
		return E_FAIL;

	return S_OK;
}

HRESULT CModel::Initialize(void * pArg)
{

	return S_OK;
}

HRESULT CModel::Render(_uint iMeshIndex)
{
	m_Meshes[iMeshIndex]->Bind_Buffers();
	m_Meshes[iMeshIndex]->Render();

	return S_OK;
}

void CModel::SetUp_Animation(_uint iAnimationIndex, _bool isLoop, _double dAnimToAnimSpeed)
{
	if (m_iCurrentAnimIndex == numeric_limits<_uint>::max()) {
		m_iCurrentAnimIndex = iAnimationIndex;
		//m_Animations[m_iCurrentAnimIndex]->InitializeTrackPosition();
	}
	else if (m_iCurrentAnimIndex == iAnimationIndex) {
		if(!m_bIsFinished)
			return;
		else {
			m_Animations[m_iCurrentAnimIndex]->InitializeTrackPosition();
		}
	}
	else {
		m_iNextAnimIndex = iAnimationIndex;
	}
	m_isLoop = isLoop;
	s_dAnimToAnimSpeed = dAnimToAnimSpeed;
}

void CModel::SetUp_Animation_Immediately(_uint iAnimationIndex, _bool isLoop, _double dAnimToAnimSpeed)
{
	if (m_iCurrentAnimIndex == numeric_limits<_uint>::max()) {
		m_iCurrentAnimIndex = iAnimationIndex;
		//m_Animations[m_iCurrentAnimIndex]->InitializeTrackPosition();
	}
	else {
		m_iNextAnimIndex = iAnimationIndex;
	}
	m_isLoop = isLoop;

	s_dAnimToAnimSpeed = dAnimToAnimSpeed;
}

_bool CModel::Play_Animation(_float fTimeDelta, _float4x4* rootBoneMatrix, _bool dontmove)
{
	CAnimation* pNextAnimation = nullptr;
	if (m_iNextAnimIndex < m_Animations.size()) {
		pNextAnimation = m_Animations[m_iNextAnimIndex];
	}
	
	if (m_bIsFinished && m_iNextAnimIndex != numeric_limits<_uint>::max()) {
		m_iCurrentAnimIndex = m_iNextAnimIndex;
		m_Animations[m_iCurrentAnimIndex]->InitializeTrackPosition();
		m_iNextAnimIndex = numeric_limits<_uint>::max();
	}

	if (m_bIsFinished && m_isLoop) {
		m_Animations[m_iCurrentAnimIndex]->InitializeTrackPosition();
	}

	// 뼈를 움직인다. CBone의 m_TransformationMatrix 행렬을 갱신한다.
	m_bIsFinished = m_Animations[m_iCurrentAnimIndex]->Update_TransformationMatrices(m_Bones, m_isLoop, fTimeDelta, pNextAnimation);

	if(dontmove)
		m_Bones[1]->Set_TransformationMatrix(XMMatrixIdentity());

	for (auto& pBone : m_Bones)
	{
		// 내 뼈의 행렬 * 부모의 컴바인드
		pBone->Update_CombinedTransformationMatrix(m_Bones, XMLoadFloat4x4(&m_PreTransformMatrix));
	}
	if(rootBoneMatrix != nullptr)
	{
		XMStoreFloat4x4(rootBoneMatrix, m_Bones[5]->Get_CombinedTransformationMatrix());
	}

	return m_bIsFinished;
}

HRESULT CModel::Bind_Material(CShader* pShader, const _char* pConstantName, aiTextureType eMaterialType, _uint iMeshIndex)
{
	_uint iMaterialIndex = m_Meshes[iMeshIndex]->Get_MaterialIndex();

	if (m_Materials[iMaterialIndex].pMaterialTextures[eMaterialType] == nullptr) {
		return E_FAIL;
	}

	return m_Materials[iMaterialIndex].pMaterialTextures[eMaterialType]->Bind_ShadeResource(pShader, pConstantName, 0);
}

HRESULT CModel::Bind_MeshBoneMatrices(CShader* pShader, const _char* pConstantName, _uint iMeshIndex)
{
	m_Meshes[iMeshIndex]->Bind_BoneMatrices(this, pShader, pConstantName);

	return S_OK;
}

const vector<string> CModel::GetAnimationNames() const
{
	vector<string> ret = {};
	for (size_t i = 0; i < m_Animations.size(); i++) {
		ret.push_back(m_Animations[i]->Get_Name());
	}
	return ret;
}

const vector<string> CModel::GetAnimationNames_By50(_uint iNum) const
{
	vector<string> ret = {};

	size_t iOffset = iNum * 50;

	for (size_t i = iOffset; i < iOffset + 50; i++) {
		if (m_Animations.size() <= i) {
			break;
		}
		ret.push_back(m_Animations[i]->Get_Name());
	}
	return ret;
}

const vector<string> CModel::GetAnimationNames_10(_uint iNum) const
{
	vector<string> ret = {};

	for (size_t i = iNum; i < iNum + 10; i++) {
		if (m_Animations.size() <= i) {
			break;
		}
		string name = to_string(i);
		name += ". "; name += m_Animations[i]->Get_Name();
		ret.push_back(name);
	}
	return ret;
}

_double CModel::GetCurrentAnimDuration() const
{
	return m_Animations[m_iCurrentAnimIndex]->Get_Duration();
}

_bool CModel::LineIntersects(_fvector vWorldRay_Pos, _fvector vWorldRay_Dir, _cmatrix worldMatrix, _float* pfDistance)
{
	for (auto* pMesh : m_Meshes) {
		if (pMesh->LineIntersects(vWorldRay_Pos, vWorldRay_Dir, worldMatrix, pfDistance)){
			return true;
		}
	}
	return false;
}

_float CModel::GetLength() const
{
	_float max = numeric_limits<_float>::min();
	if (max < GetSizeX())
		max = GetSizeX();
	if (max < GetSizeY())
		max = GetSizeY();
	if (max < GetSizeZ())
		max = GetSizeZ();

	return max;
}

_float CModel::GetSizeX() const
{
	_float min = GetMinXorYorZ(0);
	_float max = GetMaxXorYorZ(0);

	return max - min;
}

_float CModel::GetSizeY() const
{
	_float min = GetMinXorYorZ(1);
	_float max = GetMaxXorYorZ(1);

	return max - min;
}

_float CModel::GetSizeZ() const
{
	_float min = GetMinXorYorZ(2);
	_float max = GetMaxXorYorZ(2);

	return max - min;
}

_float CModel::GetMinXorYorZ(int i) const
{
	_float ret = numeric_limits<_float>::max();
	if (i < 0 || i > 2) {
		return ret;
	}
	if (i == 0)
	{
		for (auto* pMesh : m_Meshes) {
			if (ret > pMesh->GetMinX()) {
				ret = pMesh->GetMinX();
			}
		}
	}
	if (i == 1)
	{
		for (auto* pMesh : m_Meshes) {
			if (ret > pMesh->GetMinY()) {
				ret = pMesh->GetMinY();
			}
		}
	}
	if (i == 2)
	{
		for (auto* pMesh : m_Meshes) {
			if (ret > pMesh->GetMinZ()) {
				ret = pMesh->GetMinZ();
			}
		}
	}
	return ret;
}

_float CModel::GetMaxXorYorZ(int i) const
{
	_float ret = numeric_limits<_float>::min();
	if (i < 0 || i > 2) {
		return ret;
	}
	if (i == 0)
	{
		for (auto* pMesh : m_Meshes) {
			if (ret < pMesh->GetMaxX()) {
				ret = pMesh->GetMaxX();
			}
		}
	}
	if (i == 1)
	{
		for (auto* pMesh : m_Meshes) {
			if (ret < pMesh->GetMaxY()) {
				ret = pMesh->GetMaxY();
			}
		}
	}
	if (i == 2)
	{
		for (auto* pMesh : m_Meshes) {
			if (ret < pMesh->GetMaxZ()) {
				ret = pMesh->GetMaxZ();
			}
		}
	}
	return ret;
}

HRESULT CModel::SaveModel(ofstream& saveStream) const
{
	saveStream.write(reinterpret_cast<const char*>(&m_PreTransformMatrix), sizeof(m_PreTransformMatrix));
	saveStream.write(reinterpret_cast<const char*>(&m_eType), sizeof(_uint));
	
	// Bones Save
	{
		_uint iNumBones = m_Bones.size();
		saveStream.write(reinterpret_cast<const char*>(&iNumBones), sizeof(_uint));
		for (size_t i = 0; i < iNumBones; i++)
		{
			m_Bones[i]->SaveBone(saveStream);
		}
	}

	// Mesh Save
	{
		saveStream.write(reinterpret_cast<const char*>(&m_iNumMeshes), sizeof(m_iNumMeshes));
		for (size_t i = 0; i < m_iNumMeshes; i++)
		{
			m_Meshes[i]->SaveMesh(this, saveStream, m_pAIScene->mMeshes[i], XMLoadFloat4x4(&m_PreTransformMatrix));
		}
	}
	// Material Save
	{
		saveStream.write(reinterpret_cast<const char*>(&m_iNumMaterials), sizeof(m_iNumMaterials));
		_uint mtpSize = m_MaterialTexturesPathes.size();
		saveStream.write(reinterpret_cast<const char*>(&mtpSize), sizeof(mtpSize));
		for (auto& mtp : m_MaterialTexturesPathes) {
			_uint pathSize = mtp.path.size();
			saveStream.write(reinterpret_cast<const char*>(&mtp.materialIndex), sizeof(mtp.materialIndex));
			saveStream.write(reinterpret_cast<const char*>(&mtp.textureType), sizeof(mtp.textureType));
			saveStream.write(reinterpret_cast<const char*>(&pathSize), sizeof(pathSize));
			saveStream.write(reinterpret_cast<const char*>(mtp.path.c_str()), sizeof(_tchar) * mtp.path.size());
		}
	}
	// Animation Save
	{
		saveStream.write(reinterpret_cast<const char*>(&m_iNumAnimations), sizeof(_uint));

		for (size_t i = 0; i < m_iNumAnimations; i++)
		{
			m_Animations[i]->SaveAnim(saveStream);
		}
	}

	// ETC Save
	saveStream.write(reinterpret_cast<const char*>(&m_iNextAnimIndex), sizeof(_uint));

	return S_OK;
}

HRESULT CModel::LoadModel(ifstream& loadStream)
{
	_uint		iFlag = { 0 };

	/* 이전 : 모든 메시가 다 원점을 기준으로 그렺니다. */
	/* 이후 : 모델을 구성하는 모든 메시들을 각각 정해진 상태(메시를 배치하기위한 뼈대의 위치에 맞춰서)대로 미리 변환해준다.*/

	iFlag = aiProcess_ConvertToLeftHanded | aiProcessPreset_TargetRealtime_Fast;

	auto size = sizeof(m_PreTransformMatrix);
	loadStream.read(reinterpret_cast<char*>(&m_PreTransformMatrix), sizeof(m_PreTransformMatrix));
	loadStream.read(reinterpret_cast<char*>(&m_eType), sizeof(_uint));
	if (m_eType == TYPE_NONANIM)
		iFlag |= aiProcess_PreTransformVertices;

	// Bones read
	_uint iNumBones = {};
	loadStream.read(reinterpret_cast<char*>(&iNumBones), sizeof(_uint));
	for (size_t i = 0; i < iNumBones; i++)
	{
		auto* pBone = CBone::Create(loadStream);
		m_Bones.emplace_back(pBone);
	}

	// Meshes read
	{
		loadStream.read(reinterpret_cast<char*>(&m_iNumMeshes), sizeof(m_iNumMeshes));
		for (size_t i = 0; i < m_iNumMeshes; i++)
		{
			CMesh* pMesh = CMesh::Create(m_pDevice, m_pContext, this, loadStream);
			if (nullptr == pMesh)
				return E_FAIL;

			m_Meshes.emplace_back(pMesh);
		}
	}
	// Material read
	{
		loadStream.read(reinterpret_cast<char*>(&m_iNumMaterials), sizeof(m_iNumMaterials));
		_uint mtpSize = {};
		loadStream.read(reinterpret_cast<char*>(&mtpSize), sizeof(mtpSize));
		for (int i = 0; i < mtpSize; i++) {
			_uint	materialIndex = {};
			_uint	textureType = {};
			_tchar	pathName[MAX_PATH] = {};
			_uint pathSize = 0;
			loadStream.read(reinterpret_cast<char*>(&materialIndex), sizeof(materialIndex));
			loadStream.read(reinterpret_cast<char*>(&textureType), sizeof(textureType));
			loadStream.read(reinterpret_cast<char*>(&pathSize), sizeof(pathSize));
			loadStream.read(reinterpret_cast<char*>(pathName), sizeof(_tchar) * pathSize);
			_wstring finalPath = pathName;
			m_MaterialTexturesPathes.emplace_back(
				MaterialTexturesPath{ materialIndex, textureType, finalPath }
			);
		}
		for (size_t i = 0; i < m_iNumMaterials; i++) {
			_bool noItis = true;
			MESH_MATERIAL MeshMaterial{};

			for (size_t j = 0; j < m_MaterialTexturesPathes.size(); j++) {
				if (i == m_MaterialTexturesPathes[j].materialIndex) {
					MeshMaterial.pMaterialTextures[m_MaterialTexturesPathes[j].textureType] 
						= CTexture::Create(m_pDevice, m_pContext, m_MaterialTexturesPathes[j].path.c_str(), 1);
				}
			}
			m_Materials.emplace_back(MeshMaterial);
		}
	}
	// Animation read
	{
		loadStream.read(reinterpret_cast<char*>(&m_iNumAnimations), sizeof(_uint));

		for (size_t i = 0; i < m_iNumAnimations; i++)
		{
			CAnimation* pAnim = CAnimation::Create(loadStream);
			m_Animations.emplace_back(pAnim);
		}
	}

	//ETC Read
	loadStream.read(reinterpret_cast<char*>(&m_iNextAnimIndex), sizeof(_uint));

	return S_OK;
}

_bool CModel::IsPicking(const _float4x4& WorldMatrix, _float3* pOut)
{
	_bool bPicked = false;
	for (auto* pMesh : m_Meshes) {
		bPicked = pMesh->isPicking(WorldMatrix, pOut);
		if (bPicked == true) {
			return bPicked;
		}
	}
	return bPicked;
}

HRESULT CModel::Ready_Bones(const aiNode* pNode, _int iParentBoneIndex)
{
	CBone* pBone = CBone::Create(pNode, iParentBoneIndex);
	if (nullptr == pBone)
		return E_FAIL;

	m_Bones.emplace_back(pBone);

	_int iParentIndex = m_Bones.size() - 1;

	for (size_t i = 0; i < pNode->mNumChildren; i++)
	{
		Ready_Bones(pNode->mChildren[i], iParentIndex);
	}

	return S_OK;
}

HRESULT CModel::Ready_Meshes()
{
	m_iNumMeshes = m_pAIScene->mNumMeshes;

	for (size_t i = 0; i < m_iNumMeshes; i++)
	{
		CMesh*		pMesh = CMesh::Create(m_pDevice, m_pContext, this, m_pAIScene->mMeshes[i], XMLoadFloat4x4(&m_PreTransformMatrix));
		if (nullptr == pMesh)
			return E_FAIL;

		m_Meshes.emplace_back(pMesh);
	}

	return S_OK;
}

HRESULT CModel::Ready_Materials(const _char * pModelFilePath)
{
	m_iNumMaterials = m_pAIScene->mNumMaterials;

	for (size_t i = 0; i < m_iNumMaterials; i++)
	{
		MESH_MATERIAL		MeshMaterial{};

		aiMaterial* pAIMaterial = m_pAIScene->mMaterials[i];

		for (size_t j = 1; j < AI_TEXTURE_TYPE_MAX; j++)
		{			
			// pAIMaterial->GetTextureCount(j);
			aiString			strTexturePath;

			_char				szDrive[MAX_PATH] = "";
			_char				szDirectory[MAX_PATH] = "";
			_char				szFileName[MAX_PATH] = "";
			_char				szExt[MAX_PATH] = "";

			_char				szTexturePath[MAX_PATH] = "";

			if (FAILED(pAIMaterial->GetTexture(aiTextureType(j), 0, &strTexturePath)))
				continue;

			_splitpath_s(pModelFilePath, szDrive, MAX_PATH, szDirectory, MAX_PATH, nullptr, 0, nullptr, 0);
			_splitpath_s(strTexturePath.data, nullptr, 0, nullptr, 0, szFileName, MAX_PATH, szExt, MAX_PATH);

			strcpy_s(szTexturePath, szDrive);
			strcat_s(szTexturePath, szDirectory);
			strcat_s(szTexturePath, szFileName);
			strcat_s(szTexturePath, szExt);

			//0730 김선권 추가
			if (strcmp(szExt, "") == 0) {
				continue;
			}

			_tchar				szFinalPath[MAX_PATH] = TEXT("");

			MultiByteToWideChar(CP_ACP, 0, szTexturePath, strlen(szTexturePath), szFinalPath, MAX_PATH);

			MeshMaterial.pMaterialTextures[j] = CTexture::Create(m_pDevice, m_pContext, szFinalPath, 1);
			if (nullptr == MeshMaterial.pMaterialTextures[j])
				return E_FAIL;			
			else {
				MaterialTexturesPath p = {};
				p.materialIndex = i;
				p.textureType = j;
				p.path = szFinalPath;
				m_MaterialTexturesPathes.push_back(p);
			}
		}

		m_Materials.emplace_back(MeshMaterial);
	}

	return S_OK;
}

HRESULT CModel::Ready_Animations()
{
	m_iNumAnimations = m_pAIScene->mNumAnimations;

	for (size_t i = 0; i < m_iNumAnimations; i++)
	{
		CAnimation* pAnimation = CAnimation::Create(m_pAIScene->mAnimations[i], this);
		if (pAnimation == nullptr)
			return E_FAIL;

		m_Animations.emplace_back(pAnimation);
	}

	return S_OK;
}

CModel * CModel::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, TYPE eType, const _char * pModelFilePath, _fmatrix PreTransformMatrix)
{
	CModel*		pInstance = new CModel(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(eType, pModelFilePath, PreTransformMatrix)))
	{
		MSG_BOX(TEXT("Failed to Created : CModel"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CModel* CModel::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, ifstream& readStream)
{
	CModel* pInstance = new CModel(pDevice, pContext);

	if (FAILED(pInstance->LoadModel(readStream)))
	{
		MSG_BOX(TEXT("Failed to Created : CModel"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent * CModel::Clone(void * pArg)
{
	CModel*		pInstance = new CModel(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Created : CModel"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


void CModel::Free()
{
	__super::Free();

	for (auto& pAnimation : m_Animations)
		Safe_Release(pAnimation);
	m_Animations.clear();

	for (auto& pBone : m_Bones)
		Safe_Release(pBone);
	m_Bones.clear();

	for (auto& Material : m_Materials)
	{
		for (auto& pTexture : Material.pMaterialTextures)
			Safe_Release(pTexture);
	}
	m_Materials.clear();

	for (auto& pMesh : m_Meshes)
	{
		Safe_Release(pMesh);
	}
	m_Meshes.clear();
}
