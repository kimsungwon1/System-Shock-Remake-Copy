#include "Bounding.h"

CBounding::CBounding(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice { pDevice }, m_pContext{ pContext }
{
	Safe_AddRef(pDevice);
	Safe_AddRef(pContext);
}

HRESULT CBounding::Render(PrimitiveBatch<VertexPositionColor>* pBatch)
{
	return S_OK;
}

HRESULT CBounding::Initialize(CBounding::BOUNDING_DESC* pBoundingDesc)
{
	return S_OK;
}

void CBounding::Update(_fmatrix WorldMatrix)
{
	XMStoreFloat3(&m_vCenter, XMVector3TransformCoord(XMLoadFloat3(&m_vCenter), WorldMatrix));
}

void CBounding::Free()
{
	__super::Free();

	Safe_Release(m_pContext);
	Safe_Release(m_pDevice);
}