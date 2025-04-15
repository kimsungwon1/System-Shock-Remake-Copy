#include "stdafx.h"
#include "Elevator.h"

#include "GameInstance.h"

CElevator::CElevator(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CInteractives { pDevice, pContext }
{
}

CElevator::CElevator(const CElevator& Prototype)
    : CInteractives { Prototype }
{
}

HRESULT CElevator::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CElevator::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    return S_OK;
}

void CElevator::Priority_Update(_float fTimeDelta)
{
}

void CElevator::Update(_float fTimeDelta)
{
}

void CElevator::Late_Update(_float fTimeDelta)
{
    auto* pPlayer = m_pGameInstance->Get_CloneObject_ByIndex(LEVEL_GAMEPLAY, TEXT("Layer_Player"));

    if (m_pGameInstance->Get_RealDIKeyStateDown(DIK_E) && XMVectorGetX(XMVector3Length(pPlayer->GetTransformCom()->Get_State(CTransform::STATE_POSITION) - m_pTransformCom->Get_State(CTransform::STATE_POSITION)))
        < 2.f) {
        _float3 pOut = {};
        if (m_pModelCom->IsPicking(m_pTransformCom->GetWorldMat(), &pOut)) {
            // 어느 레벨로 갈 건지에 대한 선택 UI가 나와야 한다. 나중에 바꾸자.
            m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_UI"), TEXT("Prototype_GameObject_UI_Elevator"));
        }
    }
    __super::Late_Update(fTimeDelta);
}


HRESULT CElevator::Ready_Components()
{
    if (FAILED(Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Elevator"), TEXT("Model"),
        reinterpret_cast<CComponent**>(&m_pModelCom))))
        return E_FAIL;

    if (FAILED(Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxModel"), TEXT("Shader"),
        reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    return S_OK;
}

CElevator* CElevator::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CElevator* pInstance = new CElevator(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype())) {
        MSG_BOX(TEXT("Failed to Created : CElevator"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CElevator::Clone(void* pArg)
{
    CElevator* pInstance = new CElevator(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed to Cloned : CElevator"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CElevator::Free()
{
    __super::Free();
}
