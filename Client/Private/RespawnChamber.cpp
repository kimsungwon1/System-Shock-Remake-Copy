#include "stdafx.h"
#include "RespawnChamber.h"

CRespawnChamber::CRespawnChamber(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CInteractives{ pDevice, pContext }
{
}

CRespawnChamber::CRespawnChamber(const CRespawnChamber& Prototype)
    : CInteractives{ Prototype }
{
}

HRESULT CRespawnChamber::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CRespawnChamber::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    return S_OK;
}

void CRespawnChamber::Priority_Update(_float fTimeDelta)
{
}

void CRespawnChamber::Update(_float fTimeDelta)
{
}

HRESULT CRespawnChamber::Ready_Components()
{
    if (FAILED(Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_RespawnChamber"), TEXT("Model"),
        reinterpret_cast<CComponent**>(&m_pModelCom))))
        return E_FAIL;

    if (FAILED(Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxModel"), TEXT("Shader"),
        reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    return S_OK;
}

CRespawnChamber* CRespawnChamber::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CRespawnChamber* pInstance = new CRespawnChamber(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype())) {
        MSG_BOX(TEXT("Failed to Created : CRespawnChamber"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CRespawnChamber::Clone(void* pArg)
{
    CRespawnChamber* pInstance = new CRespawnChamber(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed to Cloned : CRespawnChamber"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CRespawnChamber::Free()
{
    __super::Free();
}
