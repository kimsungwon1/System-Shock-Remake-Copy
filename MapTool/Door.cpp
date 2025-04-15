#include "Client_Defines.h"

#include "Collider.h"
#include "GameInstance.h"
#include "Door.h"

CDoor::CDoor(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CImguiDesign* pGui, const _tchar* szLayerTag)
    : CTile{ pDevice, pContext, pGui,TEXT("Prototype_GameObject_Door"), TEXT("Tile"),
    TEXT(""), AllObjectType::TYPE_TILE}
{
}

CDoor::CDoor(const CDoor& Prototype)
    : CTile{ Prototype }
{
}

HRESULT CDoor::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CDoor::Initialize(void* pArg)
{
    if (FAILED(CGameObject::Initialize(pArg)))
        return E_FAIL;

    m_szModelCompName = { TEXT("Door") };

    auto* pDesc = static_cast<DATAOBJ_DESC*>(pArg);
    m_bPlaced = pDesc->bPlaced;
    m_eObjType = AllObjectType::TYPE_TILE;

    m_iData = pDesc->iData;
    /*auto getScale = m_pTransformCom->Get_Scaled();
    if (getScale.x < 1.9f) {
        m_pTransformCom->Set_Scaled(2.f, 2.f, 1.f);
    }*/

    m_szLayerTag = TypeToLayername[(_int)m_eObjType];

    if (FAILED(Ready_Components()))
        return E_FAIL;

    return S_OK;
}

void CDoor::Priority_Update(_float fTimeDelta)
{
    
}

void CDoor::Update(_float fTimeDelta)
{
    //m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix_Ptr());
}

void CDoor::Late_Update(_float fTimeDelta)
{
    m_pGameInstance->Add_RenderObject(CRenderer::RG_NONLIGHT, this);
}

HRESULT CDoor::Render()
{
    if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
        return E_FAIL;
    if (FAILED(m_pTextureCom->Bind_ShadeResource(m_pShaderCom, "g_Texture", 0)))
        return E_FAIL;
    
    if (FAILED(m_pShaderCom->Bind_RawValue("g_fTime", &m_vTime.x, sizeof(_float))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_RawValue("g_fFullTime", &m_vTime.y, sizeof(_float))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Begin(1)))
        return E_FAIL;

    if (FAILED(m_pVIBufferCom->Bind_Buffers()))
        return E_FAIL;
    if (FAILED(m_pVIBufferCom->Render()))
        return E_FAIL;

    return S_OK;
}

_bool CDoor::IsPicking(_float3* pOut)
{
    m_pGameInstance->Transform_MouseRay_ToLocalSpace(m_pTransformCom->GetWorldMat());
    
    if (m_pGameInstance->isPicked_InLocalSpace(
        XMLoadFloat3(m_vPoints), XMLoadFloat3(m_vPoints + 1), XMLoadFloat3(m_vPoints + 2),
        pOut
    )) {
        goto Compute_WorldPos;
    }
    if (m_pGameInstance->isPicked_InLocalSpace(
        XMLoadFloat3(m_vPoints), XMLoadFloat3(m_vPoints + 2), XMLoadFloat3(m_vPoints + 3),
        pOut
    )) {
        goto Compute_WorldPos;
    }
    if (m_pGameInstance->isPicked_InLocalSpace(
        XMLoadFloat3(m_vPoints + 1), XMLoadFloat3(m_vPoints + 0), XMLoadFloat3(m_vPoints + 2),
        pOut
    )) {
        goto Compute_WorldPos;
    }
    if (m_pGameInstance->isPicked_InLocalSpace(
        XMLoadFloat3(m_vPoints + 0), XMLoadFloat3(m_vPoints + 3), XMLoadFloat3(m_vPoints + 2),
        pOut
    )) {
        goto Compute_WorldPos;
    }
    return false;
Compute_WorldPos:
    XMStoreFloat3(pOut, XMVector3TransformCoord(XMLoadFloat3(pOut), XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrix_Ptr())));
    return true;
}

HRESULT CDoor::Ready_Components()
{
    if (FAILED(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Beam"),
        TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
        return E_FAIL;

    if (FAILED(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPosTex"), TEXT("Shader"),
        reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    if(FAILED(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_Buffer"), 
        reinterpret_cast<CComponent**>(& m_pVIBufferCom))))
        return E_FAIL;

    return S_OK;
}

CDoor* CDoor::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CImguiDesign* pGui, const _tchar* szLayerTag)
{
    CDoor* pInstance = new CDoor(pDevice, pContext, pGui, szLayerTag);

    if (FAILED(pInstance->Initialize_Prototype())) {
        MSG_BOX(TEXT("Failed to Created : CDoor"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CDoor::Clone(void* pArg)
{
    CDoor* pInstance = new CDoor(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed to Cloned : CDoor"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CDoor::Free()
{
    __super::Free();
    Safe_Release(m_pTextureCom);
    Safe_Release(m_pVIBufferCom);
}

HRESULT CDoor::SaveObject(ofstream& saveStream) const
{
    __super::SaveObject(saveStream);
    saveStream.write(reinterpret_cast<const char*>(&m_iData), sizeof(_int));

    return S_OK;
}

HRESULT CDoor::LoadObject(ifstream& loadStream)
{
    __super::LoadObject(loadStream);

    loadStream.read(reinterpret_cast<char*>(&m_iData), sizeof(_int));

    return S_OK;
}
