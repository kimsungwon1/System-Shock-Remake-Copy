#include "Bounding_Sphere.h"

#include "Bounding_AABB.h"
#include "Bounding_OBB.h"

CBounding_Sphere::CBounding_Sphere(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CBounding {pDevice, pContext}
{
}

HRESULT CBounding_Sphere::Initialize(CBounding::BOUNDING_DESC* pBoundingDesc)
{
    BOUNDING_SPHERE_DESC* pDesc = static_cast<BOUNDING_SPHERE_DESC*>(pBoundingDesc);

    m_pBoundingDesc = new BoundingSphere(pDesc->vCenter, pDesc->fRadius);
    m_vCenter = pDesc->vCenter;
    m_vExtents.x = pDesc->fRadius;
    m_vExtents.y = pDesc->fRadius;
    m_vExtents.z = pDesc->fRadius;

    return S_OK;
}

HRESULT CBounding_Sphere::Render(PrimitiveBatch<VertexPositionColor>* pBatch)
{
#ifdef _DEBUG
    DX::Draw(pBatch, *m_pBoundingDesc, m_isColl == true ? XMVectorSet(1.f, 0.f, 0.f, 1.f) : XMVectorSet(0.f, 1.f, 0.f, 1.f));
#endif // _DEBUG

    return S_OK;
}

_bool CBounding_Sphere::Intersect(CCollider::TYPE eColliderType, CBounding* pBounding)
{
    m_isColl = false;

    switch (eColliderType)
    {
    case CCollider::TYPE_AABB:
        m_isColl = m_pBoundingDesc->Intersects(*(dynamic_cast<CBounding_AABB*>(pBounding)->Get_Desc()));
        break;

    case CCollider::TYPE_OBB:
        m_isColl = m_pBoundingDesc->Intersects(*(dynamic_cast<CBounding_OBB*>(pBounding)->Get_Desc()));
        break;

    case CCollider::TYPE_SPHERE:
        m_isColl = m_pBoundingDesc->Intersects(*(dynamic_cast<CBounding_Sphere*>(pBounding)->Get_Desc()));
        break;
    }

    return m_isColl;
}

const _float3& CBounding_Sphere::Get_Center() const
{
    return m_pBoundingDesc->Center;
}

CBounding_Sphere* CBounding_Sphere::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CBounding::BOUNDING_DESC* pBoundingDesc)
{
    CBounding_Sphere* pInstance = new CBounding_Sphere(pDevice, pContext);

    if (FAILED(pInstance->Initialize(pBoundingDesc)))
    {
        MSG_BOX(TEXT("Failed to Created : CBounding_Sphere"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CBounding_Sphere::Free()
{
    __super::Free();
}
