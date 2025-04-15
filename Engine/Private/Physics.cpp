#include "Physics.h"

const _float CPhysics::s_fGravity = 9.8f;

CPhysics::CPhysics(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CComponent{ pDevice, pContext }
{
}

CPhysics::CPhysics(const CPhysics& Prototype)
    : CComponent{ Prototype }
{

}

HRESULT CPhysics::Initialize_Prototype()
{
    
    return S_OK;
}

HRESULT CPhysics::Initialize(void* pArg)
{
    if (nullptr == pArg)
        return S_OK;


    return S_OK;
}

void CPhysics::Update(_float fTimeDelta)
{
}

void CPhysics::PopPower(_int i)
{
    switch (i)
    {
    case 0:
        m_vPower.x = 0.f;
        break;
    case 1:
        m_vPower.y = 0.f;
        break;
    case 2:
        m_vPower.z = 0.f;
        break;
    case 3:
        m_vPower.w = 0.f;
        break;
    default:
        m_vPower = { 0.f,0.f,0.f,0.f };
        break;
    }
}

CPhysics* CPhysics::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CPhysics* pInstance = new CPhysics(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CPhysics"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CComponent* CPhysics::Clone(void* pArg)
{
    CPhysics* pInstance = new CPhysics(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed to Cloned : CPhysics"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CPhysics::Free()
{
    __super::Free();
}