#include "LightObject.h"

#include "GameInstance.h"

#include "ImguiDesign.h"

_int CLightObject::s_iNowIndex = 1;

CLightObject::CLightObject(ID3D11Device* pDevice,
    ID3D11DeviceContext* pContext, CImguiDesign* pGui, const _tchar* szLayerTag)
    : CTile(pDevice, pContext, pGui, TEXT("Prototype_GameObject_Light"), TEXT("Light"), 
        TEXT(""), AllObjectType::TYPE_Light)
{
}

CLightObject::CLightObject(const CLightObject& Prototype)
    : CTile(Prototype)
    , m_Light_desc { Prototype.m_Light_desc }
{
}

HRESULT CLightObject::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CLightObject::Initialize(void* pArg)
{
    if (FAILED(CGameObject::Initialize(pArg)))
        return E_FAIL;

    m_szModelCompName = {TEXT("")};

    auto* pDesc = static_cast<LIGHTOBJECT_DESC*>(pArg);
    m_bPlaced = pDesc->bPlaced;
    m_eObjType = AllObjectType::TYPE_Light;

    m_szLayerTag = TypeToLayername[(_int)m_eObjType];

    m_Light_desc = pDesc->light_description;

    m_Light_desc.eType = LIGHT_DESC::TYPE_POINT;

    m_pGameInstance->Add_Light(m_Light_desc, s_iNowIndex);

    m_iIndex = s_iNowIndex;
    s_iNowIndex++;

    return S_OK;
}

void CLightObject::Priority_Update(_float fTimeDelta)
{
    XMStoreFloat4(&m_Light_desc.vPosition, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
    m_pGameInstance->Change_Light(m_Light_desc, m_iIndex);
}

void CLightObject::Update(_float fTimeDelta)
{
}

void CLightObject::Late_Update(_float fTimeDelta)
{
}

HRESULT CLightObject::Render()
{
    return S_OK;
}

_bool CLightObject::IsPicking(_float3* pOut)
{
    _vector pixelPos = XMLoadFloat3(&m_pGui->Get_PixelPos());
    if (XMVectorGetX(pixelPos - m_pTransformCom->Get_State(CTransform::STATE_POSITION)) < m_Light_desc.fRange) {
        return true;
    }
    return false;
}

HRESULT CLightObject::SaveObject(ofstream& saveStream) const
{
    CToolObject::SaveObject(saveStream);

    saveStream.write(reinterpret_cast<const char*>(&m_Light_desc.vDirection), sizeof(_float4));
    saveStream.write(reinterpret_cast<const char*>(&m_Light_desc.vPosition), sizeof(_float4));
    saveStream.write(reinterpret_cast<const char*>(&m_Light_desc.fRange), sizeof(_float));

    saveStream.write(reinterpret_cast<const char*>(&m_Light_desc.vDiffuse), sizeof(_float4));
    saveStream.write(reinterpret_cast<const char*>(&m_Light_desc.vAmbient), sizeof(_float4));
    saveStream.write(reinterpret_cast<const char*>(&m_Light_desc.vSpecular), sizeof(_float4));

    return S_OK;
}

HRESULT CLightObject::LoadObject(ifstream& loadStream)
{
    CToolObject::LoadObject(loadStream);

    loadStream.read(reinterpret_cast<char*>(&m_Light_desc.vDirection), sizeof(_float4));
    loadStream.read(reinterpret_cast<char*>(&m_Light_desc.vPosition), sizeof(_float4));
    loadStream.read(reinterpret_cast<char*>(&m_Light_desc.fRange), sizeof(_float));

    loadStream.read(reinterpret_cast<char*>(&m_Light_desc.vDiffuse), sizeof(_float4));
    loadStream.read(reinterpret_cast<char*>(&m_Light_desc.vAmbient), sizeof(_float4));
    loadStream.read(reinterpret_cast<char*>(&m_Light_desc.vSpecular), sizeof(_float4));

    return S_OK;
}

HRESULT CLightObject::Set_Desc(LIGHT_DESC& desc)
{
    m_Light_desc.vDirection = desc.vDirection;
    m_Light_desc.fRange = desc.fRange;
    m_Light_desc.vDiffuse = desc.vDiffuse;

    return S_OK;
}

CLightObject* CLightObject::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CImguiDesign* pGui, const _tchar* szLayerTag)
{
    CLightObject* pInstance = new CLightObject(pDevice, pContext, pGui, szLayerTag);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CLightObject"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CLightObject::Clone(void* pArg)
{
    CLightObject* pInstance = new CLightObject(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed to Cloned : CLightObject"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CLightObject::Free()
{
    __super::Free();

    m_pGameInstance->Delete_Light(m_iIndex);
}
