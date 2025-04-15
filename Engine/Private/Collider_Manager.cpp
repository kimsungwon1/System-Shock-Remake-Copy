#include "Collider_Manager.h"
#include "Collider.h"
#include "GameObject.h"

HRESULT CCollider_Manager::Add_Collider(CCollider* pCollider)
{
    m_Colliders.push_back(pCollider);
    Safe_AddRef(pCollider);

    return S_OK;
}

HRESULT CCollider_Manager::Delete_Collider(CCollider* pCollider)
{
    for (auto pColl = m_Colliders.begin(); pColl != m_Colliders.end(); pColl++) {
        if (*pColl == pCollider) {
            Safe_Release(*pColl);
            m_Colliders.erase(pColl);
            return S_OK;
        }
    }

    return E_FAIL;
}

_bool CCollider_Manager::Intersects(CCollider* pCollider, _float3* pColliderCenterVec)
{
    for(auto * pCol : m_Colliders)
    {
        if (auto* pOwner = pCol->Get_Owner()) {
            if (pOwner->IsDead())
                continue;
        }
        if (pCol == pCollider) {
            continue;
        }
        if (pCol->Intersect(pCollider, pColliderCenterVec))
        {
            XMStoreFloat3(pColliderCenterVec, XMLoadFloat3(pCollider->Get_Center()) - XMLoadFloat3(pCol->Get_Center()));

            return true;
        }
    }
    return false;
}

HRESULT CCollider_Manager::Initialize()
{
    return S_OK;
}

CCollider_Manager* CCollider_Manager::Create()
{
    CCollider_Manager* pInstance = new CCollider_Manager();

    if (FAILED(pInstance->Initialize()))
    {
        MSG_BOX(TEXT("Failed to Cloned : CCollider_Manager"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CCollider_Manager::Free()
{
    __super::Free();

    for (auto* iter : m_Colliders) {
        Safe_Release(iter);
    }
    m_Colliders.clear();
}
