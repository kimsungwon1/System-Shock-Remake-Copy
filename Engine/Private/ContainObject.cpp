#include "ContainObject.h"
#include "GameInstance.h"
#include "PartObject.h"

CContainObject::CContainObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CGameObject { pDevice, pContext }
{
}

CContainObject::CContainObject(const CContainObject& Prototype)
    : CGameObject { Prototype }
{
}

HRESULT CContainObject::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CContainObject::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg))) {
        return E_FAIL;
    }

    return S_OK;
}

void CContainObject::Priority_Update(_float fTimeDelta)
{
}

void CContainObject::Update(_float fTimeDelta)
{
}

void CContainObject::Late_Update(_float fTimeDelta)
{
}

HRESULT CContainObject::Render()
{
    return S_OK;
}

HRESULT CContainObject::Add_PartObject(_uint iPartID, const _wstring& strPrototypeTag, void* pArg)
{
    CGameObject* pObject = m_pGameInstance->Clone_Object(strPrototypeTag, pArg);

    if (pObject == nullptr) {
        return E_FAIL;
    }

    m_vecParts[iPartID] = dynamic_cast<CPartObject*>(pObject);

    return S_OK;
}

void CContainObject::Free()
{
    __super::Free();

    for (auto& pPartObject : m_vecParts)
        Safe_Release(pPartObject);

    m_vecParts.clear();

}
