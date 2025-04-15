#include "VIBuffer_Point_Screw.h"

#include "GameObject.h"
#include "GameInstance.h"

CVIBuffer_Point_Screw::CVIBuffer_Point_Screw(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CVIBuffer_Instancing{ pDevice, pContext }
{
}

CVIBuffer_Point_Screw::CVIBuffer_Point_Screw(const CVIBuffer_Point_Screw& Prototype)
    : CVIBuffer_Instancing { Prototype }
{
}

HRESULT CVIBuffer_Point_Screw::Initialize_Prototype(const CVIBuffer_Instancing::INSTANCE_DESC& Desc)
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
		pInstanceVertices[i].vTranslation = _float4(
			m_pGameInstance->Get_Random(m_vCenterPos.x - m_vRange.x * 0.5f, m_vCenterPos.x + m_vRange.x * 0.5f),
			m_pGameInstance->Get_Random(m_vCenterPos.y - m_vRange.y * 0.5f, m_vCenterPos.y + m_vRange.y * 0.5f),
			m_pGameInstance->Get_Random(m_vCenterPos.z - m_vRange.z * 0.5f, m_vCenterPos.z + m_vRange.z * 0.5f),
			1.f);

		pInstanceVertices[i].vLifeTime = _float2(m_pGameInstance->Get_Random(m_vLifeTime.x, m_vLifeTime.y), 0.0f);
	}

	ZeroMemory(&m_InstanceInitialData, sizeof m_InstanceInitialData);
	m_InstanceInitialData.pSysMem = m_pInstanceVertices;

#pragma endregion

    return S_OK;
}

HRESULT CVIBuffer_Point_Screw::Initialize(void* pArg)
{
#pragma region INSTANCE_BUFFER

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

#pragma endregion
	auto* desc = static_cast<SCREWINSTANCE_DESC*>(pArg);

	m_fLookOffset = desc->fLookOffset;
	m_pOwner = desc->pOwner;

	return S_OK;
}

void CVIBuffer_Point_Screw::Spread(_float fTimeDelta)
{
}

void CVIBuffer_Point_Screw::Drop(_float fTimeDelta)
{
}

void CVIBuffer_Point_Screw::Screw(_float fTimeDelta)
{
	_float fRadius = XMConvertToRadians(1.f);
	_float fPerLookDistance = 0.01f;
	_float fUp = 2.6f;
	_vector vOwnerLook = m_pOwner->GetTransformCom()->Get_State(CTransform::STATE_LOOK);
	_vector vStartPos = m_pOwner->GetTransformCom()->Get_State(CTransform::STATE_POSITION) + fUp * m_pOwner->GetTransformCom()->Get_State(CTransform::STATE_UP)
		+ vOwnerLook * m_fLookOffset;
	_vector vPointLook = -fUp * 0.5f * m_pOwner->GetTransformCom()->Get_State(CTransform::STATE_UP);
	_float vPointLookLen = XMVectorGetX(XMVector3Length(vPointLook));

	//시간마다 돌리기
	m_fTimePass += fTimeDelta;
	{
		vStartPos += vPointLook;
		vPointLook = XMVector3TransformNormal(vPointLook, XMMatrixRotationAxis(vOwnerLook, -XMConvertToRadians(360.f) * m_fTimePass));
		vStartPos -= vPointLook;
	}

	D3D11_MAPPED_SUBRESOURCE SubResource = {};

	m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);

	VTXPOINTINSTANCE* pVertices = static_cast<VTXPOINTINSTANCE*>(SubResource.pData);

	for (size_t i = 0; i < m_iNumInstance; i++)
	{
		_vector vPerPos = vStartPos + vPointLook;
		_vector vNewPointLook = XMVector3TransformNormal(vPointLook, XMMatrixRotationAxis(vOwnerLook, fRadius * (_float)i));
		XMStoreFloat4(&pVertices[i].vTranslation, XMVectorSetW(vPerPos - vNewPointLook + vOwnerLook * fPerLookDistance * (_float)i, 1.f));
	}

	m_pContext->Unmap(m_pVBInstance, 0);
}

CVIBuffer_Point_Screw* CVIBuffer_Point_Screw::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const CVIBuffer_Instancing::INSTANCE_DESC& Desc)
{
	CVIBuffer_Point_Screw* pInstance = new CVIBuffer_Point_Screw(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(Desc)))
	{
		MSG_BOX(TEXT("Failed to Created : CVIBuffer_Point_Screw"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CVIBuffer_Point_Screw::Clone(void* pArg)
{
	CVIBuffer_Point_Screw* pInstance = new CVIBuffer_Point_Screw(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CVIBuffer_Point_Screw"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CVIBuffer_Point_Screw::Free()
{
	__super::Free();
}
