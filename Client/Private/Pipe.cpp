#include "stdafx.h"
#include "Pipe.h"
#include "FPSPlayer.h"
#include "GameInstance.h"

CPipe::CPipe(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CWeapon(pDevice, pContext)
{
}

CPipe::CPipe(const CWeapon& Prototype)
    : CWeapon(Prototype)
{
}

HRESULT CPipe::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CPipe::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxModel"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShader))))
        return E_FAIL;

    if (FAILED(Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Pipe"), TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModel)))) {
        return E_FAIL;
    }

    _matrix worldMat = XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrix_Ptr());
    _float4x4 resultMat = {};

    worldMat = XMMatrixIdentity();//XMMatrixRotationX(XMConvertToRadians(270.f)) * XMMatrixRotationZ(XMConvertToRadians(73.f));

    worldMat.r[3].m128_f32[0] = -0.2f;
    worldMat.r[3].m128_f32[1] = -0.35f;
    worldMat.r[3].m128_f32[2] =  0.1f;

    XMStoreFloat4x4(&resultMat, worldMat);
    m_pTransformCom->SetWorldMat(resultMat);

    m_eItemType = TYPE_Pipe;
    m_fAttackDuration = 0.1f;

    return S_OK;
}

void CPipe::Priority_Update(_float fTimeDelta)
{
}

void CPipe::Update(_float fTimeDelta)
{
}

void CPipe::Late_Update(_float fTimeDelta)
{
    _matrix socketMatrix = XMLoadFloat4x4(m_pSocketTransform);

    for (size_t i = 0; i < 3; i++) {
        socketMatrix.r[i] = XMVector3Normalize(socketMatrix.r[i]);
    }

    _matrix parentMatrix = XMLoadFloat4x4(m_pParentTransform);
    parentMatrix.r[3].m128_f32[1] += CFPSPlayer::Get_Height();
    _matrix worldMatrix = XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrix_Ptr()) * socketMatrix * parentMatrix;

    XMStoreFloat4x4(&m_WorldMatrix, worldMatrix);

    m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);
}

HRESULT CPipe::Render()
{
    return __super::Render();
}

CPipe* CPipe::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CPipe* pInstance = new CPipe(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype())) {
        MSG_BOX(TEXT("Failed to Initialize : Create CPipe"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CPipe::Clone(void* pArg)
{
    CPipe* pInstance = new CPipe(*this);

    if (FAILED(pInstance->Initialize(pArg))) {
        MSG_BOX(TEXT("Failed to Initialize : Clone CPipe"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CPipe::Free()
{
    __super::Free();
}
