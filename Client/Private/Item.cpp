#include "stdafx.h"

#include "Item.h"
#include "UnitObject.h"
#include "FPSPlayer.h"
#include "TileObject.h"

#include "UI_Info.h"

#include "GameInstance.h"

const _float CItem::s_fPickItemDistance = 3.f;

CItem::CItem(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CPartObject(pDevice, pContext)
{
}

CItem::CItem(const CItem& Prototype)
    : CPartObject(Prototype)
{
}

HRESULT CItem::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CItem::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    auto* desc = static_cast<ITEM_DESC*>(pArg);

    m_pOwner = desc->pOwner;

    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxModel"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShader))))
        return E_FAIL;

    return S_OK;
}

void CItem::Priority_Update(_float fTimeDelta)
{
}

void CItem::Update(_float fTimeDelta)
{
    m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix_Ptr());
}

void CItem::Late_Update(_float fTimeDelta)
{
    if (m_bOnInven)
        return;

    m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);
}

HRESULT CItem::Render()
{
#ifdef _DEBUG
    if(m_pColliderCom != nullptr)
        m_pColliderCom->Render();
#endif

    if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShader, "g_WorldMatrix")))
        return E_FAIL;

    if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
        return E_FAIL;
    if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
        return E_FAIL;

    _uint		iNumMeshes = m_pModel->Get_NumMeshes();

    for (size_t i = 0; i < iNumMeshes; i++)
    {
        if (FAILED(m_pModel->Bind_Material(m_pShader, "g_DiffuseTexture", aiTextureType_DIFFUSE, i)))
            return E_FAIL;
        /*if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_NormalTexture", aiTextureType_NORMALS, i)))
            return E_FAIL;*/

        if (FAILED(m_pShader->Begin(0)))
            return E_FAIL;

        if (FAILED(m_pModel->Render(i)))
            return E_FAIL;
    }

    return S_OK;
}

_bool CItem::Is_Possible_Looted(const CContainObject* pInfluencer)
{
    if (m_bOnInven)
        return false;
    _float3 fIn = {};
    _float fDistance = XMVectorGetX(XMVector3Length(pInfluencer->GetTransformCom()->Get_State(CTransform::STATE_POSITION) - m_pTransformCom->Get_State(CTransform::STATE_POSITION)));
    _bool bRet = fDistance < 2.5f && m_pColliderCom->IsPicking(m_WorldMatrix, &fIn);

    if(bRet == true && m_pPickingUI == nullptr)
    {
        CUI_Info::INFO_DESCS desc = {};
        desc.strQuote = _wstring(TEXT("Item : ")) + m_strName;
        desc.fLifeTime = -1.f;
        // 유아이 나오게
        m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_UI"), TEXT("Prototype_GameObject_UI_TextBox"), &desc);

        m_pPickingUI = m_pGameInstance->Get_CloneObject_ByIndex(LEVEL_GAMEPLAY, TEXT("Layer_UI"));
    }
    if (bRet == false && m_pPickingUI != nullptr)
    {
        m_pGameInstance->Reserve_ToDelete(LEVEL_GAMEPLAY, TEXT("Layer_UI"), m_pPickingUI);
        m_pPickingUI = nullptr;
    }

    return bRet;
}

_bool CItem::IsPicking()
{
    _float3 vOut = {};
    _vector vPosition = {};
    _bool bResult = false;
    if (m_pGameInstance->Get_RealDIMouseStateDown(MOUSEKEYSTATE::DIMK_LBUTTON)) {
        bResult = m_pColliderCom->IsPicking(m_pTransformCom->GetWorldMat(), &vOut);
    }
    vPosition = XMLoadFloat3(&vOut);

    if (XMVectorGetX(XMVector3Length(vPosition - m_pTransformCom->Get_State(CTransform::STATE_POSITION))) < s_fPickItemDistance) {
        //picked 해야 하는데...
    }


    return _bool();
}

void CItem::Set_Influencer(const CContainObject* pInfluencer, const _float4x4* pParentMatrix, const _float4x4* pSocketMatrix)
{
    m_pInfluencer = pInfluencer;
    m_pParentTransform = pParentMatrix;
    m_pSocketTransform = pSocketMatrix;
}

void CItem::DeleteInfoUI()
{
    if (m_pPickingUI == nullptr)
        return;
    m_pGameInstance->Reserve_ToDelete(LEVEL_GAMEPLAY, TEXT("Layer_UI"), m_pPickingUI);
}

void CItem::Free()
{
    __super::Free();

    Safe_Release(m_pColliderCom);
    Safe_Release(m_pShader);
    Safe_Release(m_pModel);
}