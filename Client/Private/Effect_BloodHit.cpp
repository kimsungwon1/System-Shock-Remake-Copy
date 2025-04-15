#include "stdafx.h"
#include "Effect_BloodHit.h"

#include "GameInstance.h"

CEffect_BloodHit::CEffect_BloodHit(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CGameObject { pDevice, pContext }
{
}

CEffect_BloodHit::CEffect_BloodHit(const CEffect_BloodHit& Prototype)
    : CGameObject { Prototype }
{
}

HRESULT CEffect_BloodHit::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CEffect_BloodHit::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    m_pTransformCom->Set_Scaled(0.3f, 0.3f, 0.3f);

    return S_OK;
}

void CEffect_BloodHit::Priority_Update(_float fTimeDelta)
{
    m_fFrame.x += fTimeDelta;
}

void CEffect_BloodHit::Update(_float fTimeDelta)
{
    auto* pPlayer = m_pGameInstance->Get_CloneObject_ByIndex(LEVEL_GAMEPLAY, TEXT("Layer_Player"));
    m_pTransformCom->SetLookToRightUp(pPlayer->GetTransformCom()->Get_State(CTransform::STATE_LOOK));

    if (m_fFrame.x >= m_fFrame.y) {
        m_pGameInstance->Reserve_ToDelete(LEVEL_GAMEPLAY, TEXT("Layer_Effect"), this);
    }
}

void CEffect_BloodHit::Late_Update(_float fTimeDelta)
{
    /* 직교투영을 위한 월드행렬까지 셋팅하게 된다. */
    __super::Late_Update(fTimeDelta);

    m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);
}

HRESULT CEffect_BloodHit::Render()
{
    if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
        return E_FAIL;
    _uint iFrame = _uint(6.f * m_fFrame.x / m_fFrame.y);
    if (FAILED(m_pTextureCom->Bind_ShadeResource(m_pShaderCom, "g_Texture", iFrame)))
        return E_FAIL;
    if (FAILED(m_pNormalTextureCom->Bind_ShadeResource(m_pShaderCom, "g_NormalTexture", iFrame)))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_RawValue("g_fTime", &m_fFrame.x, sizeof(_float))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_RawValue("g_fFullTime", &m_fFrame.y, sizeof(_float))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Begin(2)))
        return E_FAIL;

    if (FAILED(m_pVIBufferCom->Bind_Buffers()))
        return E_FAIL;
    if (FAILED(m_pVIBufferCom->Render()))
        return E_FAIL;

    return S_OK;
}

HRESULT CEffect_BloodHit::Ready_Components()
{
    /* FOR.Com_Shader */
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxPosNorTex"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    /* FOR.Com_Texture */
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Blood_Hit"),
        TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
        return E_FAIL;

    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Blood_Hit_Normal"),
        TEXT("Com_Texture_Normal"), reinterpret_cast<CComponent**>(&m_pNormalTextureCom))))
        return E_FAIL;

    /* FOR.Com_VIBuffer */
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
        TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
        return E_FAIL;

    return S_OK;
}

CEffect_BloodHit* CEffect_BloodHit::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CEffect_BloodHit* pInstance = new CEffect_BloodHit(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CEffect_BloodHit"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CEffect_BloodHit::Clone(void* pArg)
{
    CEffect_BloodHit* pInstance = new CEffect_BloodHit(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed to Cloned : CEffect_BloodHit"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CEffect_BloodHit::Free()
{
    __super::Free();

    Safe_Release(m_pShaderCom);
    Safe_Release(m_pTextureCom);
    Safe_Release(m_pNormalTextureCom);
    Safe_Release(m_pVIBufferCom);
}
