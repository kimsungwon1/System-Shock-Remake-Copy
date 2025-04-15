#include "stdafx.h"
#include "Effect_Blast.h"

#include "GameInstance.h"

CEffect_Blast::CEffect_Blast(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CGameObject{ pDevice, pContext }
{
}

CEffect_Blast::CEffect_Blast(const CEffect_Blast& Prototype)
    : CGameObject{ Prototype }
{
}

HRESULT CEffect_Blast::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CEffect_Blast::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    auto* pDesc = static_cast<BLAST_DESC*>(pArg);

    m_pTransformCom->Set_Scaled(pDesc->fScale, pDesc->fScale, 1.f);
    m_pOwner = pDesc->pOwner;
    //m_pTransformCom->Set_Scaled(0.3f, 0.3f, 0.3f);
    return S_OK;
}

void CEffect_Blast::Priority_Update(_float fTimeDelta)
{
    if (m_fFrame.x > m_fFrame.y) {
        m_iRealFrame = (++m_iRealFrame) % 4;
        m_fFrame.x = 0.f;
    }

    m_fFrame.x += fTimeDelta;
}

void CEffect_Blast::Update(_float fTimeDelta)
{

}

void CEffect_Blast::Late_Update(_float fTimeDelta)
{
    m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_pOwner->GetTransformCom()->Get_State(CTransform::STATE_POSITION) - m_pOwner->GetTransformCom()->Get_State(CTransform::STATE_LOOK));

    auto* pPlayer = m_pGameInstance->Get_CloneObject_ByIndex(LEVEL_GAMEPLAY, TEXT("Layer_Player"));
    m_pTransformCom->SetLookToRightUp(pPlayer->GetTransformCom()->Get_State(CTransform::STATE_LOOK));

    m_pGameInstance->Add_RenderObject(CRenderer::RG_NONLIGHT, this);
}

HRESULT CEffect_Blast::Render()
{
    if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
        return E_FAIL;

    if (FAILED(m_pTextureCom->Bind_ShadeResource(m_pShaderCom, "g_Texture", 0)))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_RawValue("g_iFrame", &m_iRealFrame, sizeof(m_iRealFrame))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Begin(6)))
        return E_FAIL;

    if (FAILED(m_pVIBufferCom->Bind_Buffers()))
        return E_FAIL;
    if (FAILED(m_pVIBufferCom->Render()))
        return E_FAIL;

    return S_OK;
}

HRESULT CEffect_Blast::Ready_Components()
{
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPosTex"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    /* FOR.Com_Texture */
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Blast"),
        TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
        return E_FAIL;

    /* FOR.Com_VIBuffer */
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
        TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
        return E_FAIL;

    return S_OK;
}

CEffect_Blast* CEffect_Blast::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CEffect_Blast* pInstance = new CEffect_Blast(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CEffect_Blast"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CEffect_Blast::Clone(void* pArg)
{
    CEffect_Blast* pInstance = new CEffect_Blast(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed to Cloned : CEffect_Blast"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CEffect_Blast::Free()
{
    __super::Free();

    Safe_Release(m_pShaderCom);
    Safe_Release(m_pTextureCom);
    Safe_Release(m_pVIBufferCom);
}
