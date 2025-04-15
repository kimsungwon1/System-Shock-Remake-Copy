#include "stdafx.h"
#include "Effect_Muzzle_Flame.h"

#include "GameInstance.h"

CEffect_Muzzle_Flame::CEffect_Muzzle_Flame(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CBlendObject{ pDevice, pContext }
{
}

CEffect_Muzzle_Flame::CEffect_Muzzle_Flame(const CEffect_Muzzle_Flame& Prototype)
    : CBlendObject{ Prototype }
{
}

HRESULT CEffect_Muzzle_Flame::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CEffect_Muzzle_Flame::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    auto* pDesc = static_cast<MUZZLE_DESC*>(pArg);

    m_pTransformCom->Set_Scaled(pDesc->fScale, pDesc->fScale, 1.f);

    m_iIndex = pDesc->iIndex;

    return S_OK;
}

void CEffect_Muzzle_Flame::Priority_Update(_float fTimeDelta)
{
    if (m_fFrame.x > m_fFrame.y) {
        //m_fFrame.x = 0.f;
        m_pGameInstance->Reserve_ToDelete(LEVEL_GAMEPLAY, TEXT("Layer_Effect"), this);
    }

    m_fFrame.x += fTimeDelta;
}

void CEffect_Muzzle_Flame::Update(_float fTimeDelta)
{

}

void CEffect_Muzzle_Flame::Late_Update(_float fTimeDelta)
{
    auto* pPlayer = m_pGameInstance->Get_CloneObject_ByIndex(LEVEL_GAMEPLAY, TEXT("Layer_Player"));
    m_pTransformCom->SetLookToRightUp(pPlayer->GetTransformCom()->Get_State(CTransform::STATE_LOOK));

    m_pGameInstance->Add_RenderObject(CRenderer::RG_BLEND, this);
}

HRESULT CEffect_Muzzle_Flame::Render()
{
    if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_RawValue("g_fFrame", &m_fFrame, sizeof(_float2))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_RawValue("g_iFrame", &m_iIndex, sizeof(_int))))
        return E_FAIL;

    _int iFrame = _int(m_fFrame.x / (m_fFrame.y) * 2.f);
    if (FAILED(m_pTextureCom->Bind_ShadeResource(m_pShaderCom, "g_Texture", iFrame)))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Begin(8)))
        return E_FAIL;

    if (FAILED(m_pVIBufferCom->Bind_Buffers()))
        return E_FAIL;
    if (FAILED(m_pVIBufferCom->Render()))
        return E_FAIL;

    return S_OK;
}

HRESULT CEffect_Muzzle_Flame::Ready_Components()
{
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPosTex"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    /* FOR.Com_Texture */
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Muzzle_Flame"),
        TEXT("Com_Texture1"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
        return E_FAIL;

    /* FOR.Com_VIBuffer */
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
        TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
        return E_FAIL;

    return S_OK;
}

CEffect_Muzzle_Flame* CEffect_Muzzle_Flame::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CEffect_Muzzle_Flame* pInstance = new CEffect_Muzzle_Flame(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CEffect_Muzzle_Flame"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CEffect_Muzzle_Flame::Clone(void* pArg)
{
    CEffect_Muzzle_Flame* pInstance = new CEffect_Muzzle_Flame(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed to Cloned : CEffect_Muzzle_Flame"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CEffect_Muzzle_Flame::Free()
{
    __super::Free();

    Safe_Release(m_pShaderCom);
    Safe_Release(m_pTextureCom);
    Safe_Release(m_pVIBufferCom);
}
