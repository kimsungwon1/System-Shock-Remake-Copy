#include "stdafx.h"
#include "Client_Defines.h"
#include "Sword.h"

CSword::CSword(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CWeapon { pDevice, pContext }
{
}

CSword::CSword(const CSword& Prototype)
    : CWeapon { Prototype }
{
}

HRESULT CSword::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CSword::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg))) {
		return E_FAIL;
	}

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxModel"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShader))))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_ForkLift"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModel))))
		return E_FAIL;

	m_pTransformCom->Set_Scaled(0.1f, 0.1f, 0.1f);
	//m_pTransformCom->Rotation(0.0f, XMConvertToRadians(90.0f), 0.1f);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(0.5f, 0.f, 0.f, 1.f));

    return S_OK;
}

void CSword::Priority_Update(_float fTimeDelta)
{
}

void CSword::Update(_float fTimeDelta)
{
}

void CSword::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);
}

HRESULT CSword::Render()
{
	__super::Render();

    return S_OK;
}

CSword* CSword::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CSword* pInstance = new CSword(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CSword"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CSword::Clone(void* pArg)
{
	CSword* pInstance = new CSword(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Created : CSword"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSword::Free()
{
    __super::Free();
}
