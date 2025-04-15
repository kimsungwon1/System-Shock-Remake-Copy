#include "stdafx.h"
#include "Thunder.h"

#include "UnitObject.h"
#include "FPSPlayer.h"

#include "GameInstance.h"

CThunder::CThunder(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CAmmunition{ pDevice, pContext }
{
}

CThunder::CThunder(const CThunder& Prototype)
    : CAmmunition{ Prototype }
{
}

HRESULT CThunder::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CThunder::Initialize(void* pArg)
{
    if (CItem::Initialize(pArg))
        return E_FAIL;

    auto* desc = static_cast<Ammunition_Desc*>(pArg);

    m_bShot = true;
    m_pShooter = desc->pShooter;
    m_iDamage = desc->iDamage;

    m_pTransformCom->SetLookToRightUp(XMLoadFloat3(&desc->vTargetPos));

    if (Ready_Components()) {
        return E_FAIL;
    }

    return S_OK;
}

void CThunder::Priority_Update(_float fTimeDelta)
{
    m_pTransformCom->SetLookToRightUp(m_pShooter->GetTransformCom()->Get_State(CTransform::STATE_LOOK));
    m_vDamageSchedule.x += fTimeDelta;
    m_fTime += fTimeDelta;

    m_pTransformCom->Turn(m_pTransformCom->Get_State(CTransform::STATE_LOOK), m_fTime);
}

void CThunder::Update(_float fTimeDelta)
{
    if (m_pShooter->IsDead()) {
        m_pGameInstance->Reserve_ToDelete(LEVEL_GAMEPLAY, TEXT("Layer_Effect"), this);
    }
}

void CThunder::Late_Update(_float fTimeDelta)
{
    CThunder::IsCollideAndHurt(fTimeDelta);

    m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);
}

HRESULT CThunder::Render()
{
//#ifdef _DEBUG
//    m_pColliderCom->Render();
//#endif
    _float4x4 worldMatrix = m_pTransformCom->GetWorldMat();
    for (int i = 0; i < 3; i++) {
        if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", &worldMatrix))) {
            return E_FAIL;
        }
        if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
            return E_FAIL;
        if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
            return E_FAIL;

        _uint iNumMeshes = m_pModel->Get_NumMeshes();

        for (size_t i = 0; i < iNumMeshes; i++)
        {
            if (FAILED(m_pModel->Bind_Material(m_pShader, "g_DiffuseTexture", aiTextureType_DIFFUSE, i)))
                return E_FAIL;
            /*if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_NormalTexture", aiTextureType_NORMALS, i)))
                return E_FAIL;*/

            if (FAILED(m_pShader->Begin(1)))
                return E_FAIL;

            if (FAILED(m_pModel->Render(i)))
                return E_FAIL;
        }
        XMStoreFloat4x4(&worldMatrix, XMLoadFloat4x4(&worldMatrix) * XMMatrixRotationAxis(m_pTransformCom->Get_State(CTransform::STATE_LOOK), XMConvertToRadians(120.f)));
    }

    return S_OK;
}

HRESULT CThunder::Ready_Components()
{
    if (FAILED(Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Thunder"), TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModel)))) {
        return E_FAIL;
    }

    return S_OK;
}

_bool CThunder::IsCollideAndHurt(_float fTimeDelta)
{
    auto* pPlayerList = m_pGameInstance->Get_RefGameObjects(LEVEL_GAMEPLAY, TEXT("Layer_Player"));
    //auto* pTileList = m_pGameInstance->Get_RefGameObjects(LEVEL_GAMEPLAY, TEXT("Layer_Tile"));

    
    for (auto* pPlayer : *pPlayerList) {
        auto* dynamy = dynamic_cast<CFPSPlayer*>(pPlayer);
        _vector vToPlayer = dynamy->GetTransformCom()->Get_State(CTransform::STATE_POSITION) + dynamy->GetTransformCom()->Get_State(CTransform::STATE_UP)
            - m_pTransformCom->Get_State(CTransform::STATE_POSITION);
        _vector vLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
        _float cos = XMVectorGetX(XMVector3Dot(vToPlayer, vLook)) / XMVectorGetX(XMVector3Length(vToPlayer)) / XMVectorGetX(XMVector3Length(vLook));
        _float sin = sqrtf(1 - powf(cos, 2.f));
        _float fDistanceFromThunder = sin * XMVectorGetX(XMVector3Length(vToPlayer));

        if (!dynamy->IsDead() && fDistanceFromThunder < 2.f && dynamy != m_pShooter) {
            // dynamy- damaged
            if(m_vDamageSchedule.x >= m_vDamageSchedule.y)
            {
                dynamy->ReceiveDamage(m_iDamage);
                m_vDamageSchedule.x = 0.f;
            }
            return true;
        }
    }
}

CThunder* CThunder::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CThunder* pInstance = new CThunder(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype())) {
        MSG_BOX(TEXT("Failed to Initialize : Create CThunder"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CThunder::Clone(void* pArg)
{
    CThunder* pInstance = new CThunder(*this);

    if (FAILED(pInstance->Initialize(pArg))) {
        MSG_BOX(TEXT("Failed to Initialize : Clone CThunder"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CThunder::Free()
{
    __super::Free();
}
