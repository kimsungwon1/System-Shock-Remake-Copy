#include "VIBuffer_Point_InvenSlot.h"

#include "GameInstance.h"

CVIBuffer_Point_InvenSlot::CVIBuffer_Point_InvenSlot(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CVIBuffer_Instancing{pDevice, pContext}
{
}

CVIBuffer_Point_InvenSlot::CVIBuffer_Point_InvenSlot(const CVIBuffer_Point_InvenSlot& Prototype)
	: CVIBuffer_Instancing { Prototype }
{
}

HRESULT CVIBuffer_Point_InvenSlot::Initialize_Prototype(const CVIBuffer_Instancing::INSTANCE_DESC& Desc)
{
	if (FAILED(__super::Initialize_Prototype(Desc)))
		return E_FAIL;

	m_iNumVertexBuffers = 2;
	m_iNumVertices = 1;
	m_iVertexStride = sizeof(VTXPOINT);
	m_iNumIndices = m_iNumInstance;
	m_iIndexStride = 2;
	m_eIndexFormat = DXGI_FORMAT_R16_UINT;
	m_eTopology = D3D_PRIMITIVE_TOPOLOGY_POINTLIST;
	m_iInstanceStride = sizeof(VTXPOINTINSTANCE);
	m_iIndexCountPerInstance = 1;

#pragma region VERTEX_BUFFER
	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);
	m_BufferDesc.ByteWidth = m_iVertexStride * m_iNumVertices;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT; /* 정적버퍼로 생성한다. */
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = m_iVertexStride;

	/* 정점버퍼에 채워줄 값들을 만들기위해서 임시적으로 공간을 할당한다. */
	VTXPOINT* pVertices = new VTXPOINT[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTXPOINT) * m_iNumVertices);

	_float	fScale = m_pGameInstance->Get_Random(m_vSize.x, m_vSize.y);

	pVertices->vPosition = _float3(0.f, 0.f, 0.f);
	pVertices->vPSize = _float2(fScale, fScale);


	ZeroMemory(&m_InitialData, sizeof m_InitialData);
	m_InitialData.pSysMem = pVertices;

	/* 정점버퍼를 생성한다. */
	if (FAILED(__super::Create_Buffer(&m_pVB)))
		return E_FAIL;

	Safe_Delete_Array(pVertices);

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

	_ushort* pIndices = new _ushort[m_iNumIndices];
	ZeroMemory(pIndices, sizeof(_ushort) * m_iNumIndices);

	ZeroMemory(&m_InitialData, sizeof m_InitialData);
	m_InitialData.pSysMem = pIndices;

	/* 인덱스버퍼를 생성한다. */
	if (FAILED(__super::Create_Buffer(&m_pIB)))
		return E_FAIL;

	Safe_Delete_Array(pIndices);

#pragma endregion

#pragma region INSTANCE_BUFFER

	ZeroMemory(&m_InstanceBufferDesc, sizeof m_InstanceBufferDesc);
	m_InstanceBufferDesc.ByteWidth = m_iInstanceStride * m_iNumInstance;
	m_InstanceBufferDesc.Usage = D3D11_USAGE_DYNAMIC; /* 정적버퍼로 생성한다. */
	m_InstanceBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_InstanceBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	m_InstanceBufferDesc.MiscFlags = 0;
	m_InstanceBufferDesc.StructureByteStride = m_iInstanceStride;

	m_pInstanceVertices = new VTXPOINTINSTANCE[m_iNumInstance];
	ZeroMemory(m_pInstanceVertices, sizeof(VTXPOINTINSTANCE) * m_iNumInstance);

	VTXPOINTINSTANCE* pInstanceVertices = static_cast<VTXPOINTINSTANCE*>(m_pInstanceVertices);

	for (size_t i = 0; i < m_iNumInstance; i++)
	{
		pInstanceVertices[i].vRight = _float4(1.f, 0.f, 0.f, 0.f);
		pInstanceVertices[i].vUp = _float4(0.f, 1.f, 0.f, 0.f);
		pInstanceVertices[i].vLook = _float4(0.f, 0.f, 1.f, 0.f);
		pInstanceVertices[i].vTranslation = _float4(0.f, 0.f, 0.f, 1.f);

		pInstanceVertices[i].vLifeTime = _float2(m_pGameInstance->Get_Random(m_vLifeTime.x, m_vLifeTime.y), 0.0f);
	}

	ZeroMemory(&m_InstanceInitialData, sizeof m_InstanceInitialData);
	m_InstanceInitialData.pSysMem = m_pInstanceVertices;

#pragma endregion

	return S_OK;
}

HRESULT CVIBuffer_Point_InvenSlot::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	auto* desc = static_cast<INVENSLOTINSTANCE_DESC*>(pArg);

	m_vScale = desc->vScale;
	for (_int i = 0; i < 5; i++) {
		for (_int j = 0; j < 12; j++) {
			m_arrSlotPoses[i][j] = desc->arrSlotPoses[i][j];
		}
	}

	D3D11_MAPPED_SUBRESOURCE SubResource = {};

	m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);

	VTXPOINTINSTANCE* pVertices = static_cast<VTXPOINTINSTANCE*>(SubResource.pData);

	_int iIndex = 0;
	for (size_t i = 0; i < 5; i++)
	{
		for (size_t j = 0; j < 12; j++)
		{
			iIndex = i * 12 + j;
			XMStoreFloat4(&pVertices[iIndex].vRight, XMLoadFloat4(&pVertices[iIndex].vRight) * m_vScale.x);
			XMStoreFloat4(&pVertices[iIndex].vUp, XMLoadFloat4(&pVertices[iIndex].vUp) * m_vScale.y);

			XMStoreFloat4(&pVertices[iIndex].vTranslation,
				XMVectorSet(m_arrSlotPoses[i][j].x - m_vScale.x * 0.5f, -m_arrSlotPoses[i][j].y + m_vScale.y * 0.5f, 0.f, 1.f));
		}
	}

	m_pContext->Unmap(m_pVBInstance, 0);

	return S_OK;
}

CVIBuffer_Point_InvenSlot* CVIBuffer_Point_InvenSlot::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const CVIBuffer_Instancing::INSTANCE_DESC& Desc)
{
	CVIBuffer_Point_InvenSlot* pInstance = new CVIBuffer_Point_InvenSlot(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(Desc)))
	{
		MSG_BOX(TEXT("Failed to Created : CVIBuffer_Point_InvenSlot"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CVIBuffer_Point_InvenSlot::Clone(void* pArg)
{
	CVIBuffer_Point_InvenSlot* pInstance = new CVIBuffer_Point_InvenSlot(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CVIBuffer_Point_InvenSlot"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CVIBuffer_Point_InvenSlot::Free()
{
	__super::Free();
}
