#include "stdafx.h"
#include "Particle_BossScrew.h"

#include "VIBuffer_Point_Screw.h"
#include "Ammunition.h"
#include "CortexReaver.h"
#include "FPSPlayer.h"

#include "GameInstance.h"

CParticle_BossScrew::CParticle_BossScrew(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CGameObject{ pDevice, pContext }
{
}

CParticle_BossScrew::CParticle_BossScrew(const CParticle_BossScrew& Prototype)
    : CGameObject { Prototype }
{
}

HRESULT CParticle_BossScrew::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CParticle_BossScrew::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    auto* pDesc = static_cast<CAmmunition::Ammunition_Desc*>(pArg);
    
    m_iDamage = pDesc->iDamage;
    CVIBuffer_Point_Screw::SCREWINSTANCE_DESC pointDesc = {};
    pointDesc.pOwner = pDesc->pShooter;
    pointDesc.fLookOffset = 1.f;

    m_pOwner = dynamic_cast<const CCortexReaver*>(pDesc->pShooter);
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Particle_Screw"),
        TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom), &pointDesc)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    return S_OK;
}

void CParticle_BossScrew::Priority_Update(_float fTimeDelta)
{
}

void CParticle_BossScrew::Update(_float fTimeDelta)
{
    m_pVIBufferCom->Screw(fTimeDelta);

    m_vDamageSchedule.x += fTimeDelta;

    auto* pPlayerList = m_pGameInstance->Get_RefGameObjects(LEVEL_GAMEPLAY, TEXT("Layer_Player"));

    for (auto* pPlayer : *pPlayerList) {
        auto* dynamy = dynamic_cast<CFPSPlayer*>(pPlayer);
        _vector vToPlayer = dynamy->GetTransformCom()->Get_State(CTransform::STATE_POSITION) + dynamy->GetTransformCom()->Get_State(CTransform::STATE_UP)
            - m_pOwner->GetTransformCom()->Get_State(CTransform::STATE_POSITION);
        _vector vLook = m_pOwner->GetTransformCom()->Get_State(CTransform::STATE_LOOK);
        _float cos = XMVectorGetX(XMVector3Dot(vToPlayer, vLook)) / XMVectorGetX(XMVector3Length(vToPlayer)) / XMVectorGetX(XMVector3Length(vLook));
        _float sin = sqrtf(1 - powf(cos, 2.f));
        _float fDistanceFromThunder = sin * XMVectorGetX(XMVector3Length(vToPlayer));

        if (!dynamy->IsDead() && fDistanceFromThunder < 2.f) {
            // dynamy- damaged
            if (m_vDamageSchedule.x >= m_vDamageSchedule.y)
            {
                dynamy->ReceiveDamage(m_iDamage);
                m_vDamageSchedule.x = 0.f;
            }
        }
    }
}

void CParticle_BossScrew::Late_Update(_float fTimeDelta)
{
    __super::Late_Update(fTimeDelta);

    if (m_pOwner->IsDead()) {
        m_pGameInstance->Reserve_ToDelete(LEVEL_GAMEPLAY, TEXT("Layer_Thunder"), this);
    }

    m_pGameInstance->Add_RenderObject(CRenderer::RG_NONLIGHT, this);
}

HRESULT CParticle_BossScrew::Render()
{
    if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
        return E_FAIL;
    if (FAILED(m_pTextureCom->Bind_ShadeResource(m_pShaderCom, "g_Texture", 0)))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_RawValue("g_vCamPosition", &m_pGameInstance->Get_CamPosition_Float4(), sizeof(_float4))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Begin(1)))
        return E_FAIL;


    if (FAILED(m_pVIBufferCom->Bind_Buffers()))
        return E_FAIL;
    if (FAILED(m_pVIBufferCom->Render()))
        return E_FAIL;

    return S_OK;
}

HRESULT CParticle_BossScrew::Ready_Components()
{
    /* FOR.Com_Shader */
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxPointInstance"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    /* FOR.Com_Texture */
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Bullet_Hit"),
        TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
        return E_FAIL;
    return S_OK;
}

CParticle_BossScrew* CParticle_BossScrew::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CParticle_BossScrew* pInstance = new CParticle_BossScrew(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CParticle_BossScrew"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CParticle_BossScrew::Clone(void* pArg)
{
    CParticle_BossScrew* pInstance = new CParticle_BossScrew(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed to Cloned : CParticle_BossScrew"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CParticle_BossScrew::Free()
{
    __super::Free();

    Safe_Release(m_pShaderCom);
    Safe_Release(m_pTextureCom);
    Safe_Release(m_pVIBufferCom);
}
