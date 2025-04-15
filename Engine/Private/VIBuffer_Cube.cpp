#include "VIBuffer_Cube.h"

CVIBuffer_Cube::CVIBuffer_Cube(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CVIBuffer { pDevice, pContext }
{
}

CVIBuffer_Cube::CVIBuffer_Cube(const CVIBuffer_Cube& Prototype)
	:CVIBuffer { Prototype }
{
}

HRESULT CVIBuffer_Cube::Initialize_Prototype(_vector scale)
{
	m_iNumVertexBuffers = 1;
	m_iNumVertices = 8;
	m_iVertexStride = sizeof(VTXMESH);
	m_iNumIndices = 36;
	m_iIndexStride = 4;
	m_eIndexFormat = DXGI_FORMAT_R32_UINT;
	m_eTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	m_pVerticesPos = new _float3[m_iNumVertices];

	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);
	m_BufferDesc.ByteWidth = m_iVertexStride * m_iNumVertices;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT; /* 정적버퍼로 생성한다. */
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = m_iVertexStride;

	VTXMESH*	pVertices = new VTXMESH[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTXMESH) * m_iNumVertices);
	
	_float x = scale.m128_f32[0];
	_float y = scale.m128_f32[1];
	_float z = scale.m128_f32[2];

	pVertices[0].vPosition = m_pVerticesPos[0] = _float3(-x * 0.5f, y * 0.5f, z * -0.5f);
	pVertices[0].vTexcoord = _float2(0.f,0.f);

	pVertices[1].vPosition = m_pVerticesPos[1] = _float3(x * 0.5f, y * 0.5f, z * -0.5f);
	pVertices[1].vTexcoord = _float2(0.f, 0.f);

	pVertices[2].vPosition = m_pVerticesPos[2] = _float3(x * 0.5f, y * -0.5f, z * -0.5f);
	pVertices[2].vTexcoord = _float2(0.f, 0.f);

	pVertices[3].vPosition = m_pVerticesPos[3] = _float3(-x * 0.5f, y * -0.5f, z * -0.5f);
	pVertices[3].vTexcoord = _float2(0.f, 0.f);

	pVertices[4].vPosition = m_pVerticesPos[4] = _float3(-x * 0.5f, y * 0.5f, z * 0.5f);
	pVertices[4].vTexcoord = _float2(0.f, 0.f);

	pVertices[5].vPosition = m_pVerticesPos[5] = _float3(x * 0.5f, y * 0.5f, z * 0.5f);
	pVertices[5].vTexcoord = _float2(0.f, 0.f);

	pVertices[6].vPosition = m_pVerticesPos[6] = _float3(x * 0.5f, -y * 0.5f, z * 0.5f);
	pVertices[6].vTexcoord = _float2(0.f, 0.f);

	pVertices[7].vPosition = m_pVerticesPos[7] = _float3(-x * 0.5f, -y * 0.5f, z * 0.5f);
	pVertices[7].vTexcoord = _float2(0.f, 0.f);

	ZeroMemory(&m_InitialData, sizeof m_InitialData);
	m_InitialData.pSysMem = pVertices;

	/* 정점버퍼를 생성한다. */
	if (FAILED(__super::Create_Buffer(&m_pVB)))
		return E_FAIL;

	Safe_Delete_Array(pVertices);

// Index Buffer
	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);
	m_BufferDesc.ByteWidth = m_iIndexStride * m_iNumIndices;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT; /* 정적버퍼로 생성한다. */
	m_BufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = m_iIndexStride;

	_ushort*		pIndices = new _ushort[m_iNumIndices];
	ZeroMemory(pIndices, sizeof(_ushort) * m_iNumIndices);

	pIndices[0] = 1;	pIndices[1] = 5;	pIndices[2] = 6;
	pIndices[3] = 1;	pIndices[4] = 6;	pIndices[5] = 2;

	pIndices[6] = 4;	pIndices[7] = 0;	pIndices[8] = 3;
	pIndices[9] = 4;	pIndices[10] = 3;	pIndices[11] = 7;
	
	pIndices[12] = 4;	pIndices[13] = 5;	pIndices[14] = 1;
	pIndices[15] = 4;	pIndices[16] = 1;	pIndices[17] = 0;

	pIndices[18] = 3;	pIndices[19] = 2;	pIndices[20] = 6;
	pIndices[21] = 3;	pIndices[22] = 6;	pIndices[23] = 7;

	pIndices[24] = 5;	pIndices[25] = 4;	pIndices[26] = 7;
	pIndices[27] = 5;	pIndices[28] = 7;	pIndices[29] = 6;

	pIndices[30] = 0;	pIndices[31] = 1;	pIndices[32] = 2;
	pIndices[33] = 0;	pIndices[34] = 2;	pIndices[35] = 3;

	ZeroMemory(&m_InitialData, sizeof m_InitialData);
	m_InitialData.pSysMem = pIndices;

	/* 인덱스버퍼를 생성한다. */
	if (FAILED(__super::Create_Buffer(&m_pIB)))
		return E_FAIL;

	Safe_Delete_Array(pIndices);

	return S_OK;
}

HRESULT CVIBuffer_Cube::Initialize(void* pArg)
{
	return S_OK;
}

void CVIBuffer_Cube::UpdateRotation()
{
}

CVIBuffer_Cube* CVIBuffer_Cube::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _vector scale)
{
	CVIBuffer_Cube*		pInstance = new CVIBuffer_Cube(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(scale)))
	{
		MSG_BOX(TEXT("Failed to Created : CVIBuffer_Cube"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent* CVIBuffer_Cube::Clone(void* pArg)
{
	CVIBuffer_Cube* pInstance = new CVIBuffer_Cube(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Created : CVIBuffer_Cube"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CVIBuffer_Cube::Free()
{
	__super::Free();
}
