#include "stdafx.h"
#include "Effect_Muzzle.h"
#include "Effect_Muzzle_Flame.h"

#include "GameInstance.h"

CEffect_Muzzle::CEffect_Muzzle(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CBlendObject{ pDevice, pContext }
{
}

CEffect_Muzzle::CEffect_Muzzle(const CEffect_Muzzle& Prototype)
    : CBlendObject{ Prototype }
{
}

HRESULT CEffect_Muzzle::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CEffect_Muzzle::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    auto* pDesc = static_cast<MUZZLE_DESC*>(pArg);
    m_fScale = pDesc->fScale;

    m_pTransformCom->Set_Scaled(pDesc->fScale, pDesc->fScale, 1.f);
    
    m_pOwner = pDesc->pOwner;
    
    return S_OK;
}

void CEffect_Muzzle::Priority_Update(_float fTimeDelta)
{
    if (m_fFrame.x > m_fFrame.z) {
        //m_fFrame.x = 0.f;
        m_pGameInstance->Reserve_ToDelete(LEVEL_GAMEPLAY, TEXT("Layer_Effect"), this);
        for (_int i = 0; i < 4; i++)
        {
            m_vRandPos[i].x = m_pGameInstance->Get_Random(-0.1f, 0.1f);
            m_vRandPos[i].y = m_pGameInstance->Get_Random(-0.1f, 0.1f);
            make_Flame(i);
        }
    }

    m_fFrame.x += fTimeDelta;
}

void CEffect_Muzzle::Update(_float fTimeDelta)
{

}

void CEffect_Muzzle::Late_Update(_float fTimeDelta)
{
    auto* pPlayer = m_pGameInstance->Get_CloneObject_ByIndex(LEVEL_GAMEPLAY, TEXT("Layer_Player"));
    m_pTransformCom->SetLookToRightUp(pPlayer->GetTransformCom()->Get_State(CTransform::STATE_LOOK));

    

    m_pGameInstance->Add_RenderObject(CRenderer::RG_BLEND, this);
}

HRESULT CEffect_Muzzle::Render()
{
    if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_RawValue("g_fFrame", &m_fFrame, sizeof(_float2))))
        return E_FAIL;

    _int iFrame = _int(m_fFrame.x / (m_fFrame.y) * 3.f);
    if (FAILED(m_pTextureCom->Bind_ShadeResource(m_pShaderCom, "g_Texture", iFrame)))
        return E_FAIL;
    
    if (FAILED(m_pShaderCom->Begin(0)))
        return E_FAIL;

    if (FAILED(m_pVIBufferCom->Bind_Buffers()))
        return E_FAIL;
    if (FAILED(m_pVIBufferCom->Render()))
        return E_FAIL;

    return S_OK;
}

HRESULT CEffect_Muzzle::make_Flame(_int i)
{
    auto vTexPos = m_vRandPos[i];
    auto* pPlayer = m_pGameInstance->Get_CloneObject_ByIndex(LEVEL_GAMEPLAY, TEXT("Layer_Player"));

    _vector vLook = pPlayer->GetTransformCom()->Get_State(CTransform::STATE_POSITION) - m_pTransformCom->Get_State(CTransform::STATE_POSITION);
    _vector vRight = XMVector3Cross(vLook, m_pTransformCom->Get_State(CTransform::STATE_UP));

    _vector newPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION) + vTexPos.x * vRight + vTexPos.y * m_pTransformCom->Get_State(CTransform::STATE_UP);

    CEffect_Muzzle_Flame::MUZZLE_DESC desc = {};
    desc.fRotationPerSec = 10.f; desc.fSpeedPerSec = 10.f; desc.fScale = m_fScale;
    desc.pOwner = this; desc.iIndex = i;

    XMStoreFloat3((_float3*)desc.transMat.m[3], newPos);

    if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Muzzle_Flame"), &desc)))
        return E_FAIL;
    return S_OK;
}

HRESULT CEffect_Muzzle::Ready_Components()
{
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPosTex"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    /* FOR.Com_Texture */
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Muzzle_Flash"),
        TEXT("Com_Texture1"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
        return E_FAIL;

    /* FOR.Com_VIBuffer */
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
        TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
        return E_FAIL;

    return S_OK;
}

CEffect_Muzzle* CEffect_Muzzle::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CEffect_Muzzle* pInstance = new CEffect_Muzzle(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CEffect_Muzzle"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CEffect_Muzzle::Clone(void* pArg)
{
    CEffect_Muzzle* pInstance = new CEffect_Muzzle(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed to Cloned : CEffect_Muzzle"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CEffect_Muzzle::Free()
{
    __super::Free();

    Safe_Release(m_pShaderCom);
    Safe_Release(m_pTextureCom);
    Safe_Release(m_pVIBufferCom);
}
