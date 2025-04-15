#include "Card.h"

#include "Collider.h"
#include "GameInstance.h"
#include "Door.h"

CCard::CCard(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CImguiDesign* pGui, const _tchar* szLayerTag)
    : CTile{ pDevice, pContext, pGui,TEXT("Prototype_GameObject_Card"), TEXT("Item"),
    TEXT(""), AllObjectType::TYPE_ITEM}
{
}

CCard::CCard(const CCard& Prototype)
    : CTile{ Prototype }
{
}

HRESULT CCard::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CCard::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    auto* pDesc = static_cast<CDoor::DATAOBJ_DESC*>(pArg);
    m_bPlaced = pDesc->bPlaced;
    m_eObjType = AllObjectType::TYPE_ITEM;
    m_iData = pDesc->iData;

    m_szLayerTag = TypeToLayername[(_int)m_eObjType];

    if (FAILED(Ready_Components()))
        return E_FAIL;

    return S_OK;
}

void CCard::Priority_Update(_float fTimeDelta)
{
    
}

void CCard::Update(_float fTimeDelta)
{
    //m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix_Ptr());
}

void CCard::Late_Update(_float fTimeDelta)
{
    m_pGameInstance->Add_RenderObject(CRenderer::RG_NONLIGHT, this);
}

HRESULT CCard::Render()
{
    return __super::Render();
}


HRESULT CCard::Ready_Components()
{
    
    return S_OK;
}

CCard* CCard::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CImguiDesign* pGui, const _tchar* szLayerTag)
{
    CCard* pInstance = new CCard(pDevice, pContext, pGui, szLayerTag);

    if (FAILED(pInstance->Initialize_Prototype())) {
        MSG_BOX(TEXT("Failed to Created : CCard"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CCard::Clone(void* pArg)
{
    CCard* pInstance = new CCard(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed to Cloned : CCard"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

HRESULT CCard::SaveObject(ofstream& saveStream) const
{
    __super::SaveObject(saveStream);
    saveStream.write(reinterpret_cast<const char*>(&m_iData), sizeof(_int));

    return S_OK;
}

HRESULT CCard::LoadObject(ifstream& loadStream)
{
    __super::LoadObject(loadStream);

    loadStream.read(reinterpret_cast<char*>(&m_iData), sizeof(_int));

    return S_OK;
}


void CCard::Free()
{
    __super::Free();
}