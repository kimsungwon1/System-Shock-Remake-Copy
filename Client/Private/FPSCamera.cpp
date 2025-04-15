#include "stdafx.h"
#include "Client_Defines.h"
#include "FPSCamera.h"

#include "GameInstance.h"

CFPSCamera::CFPSCamera(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CCamera{ pDevice, pContext }
{
}

CFPSCamera::CFPSCamera(const CFPSCamera& Prototype)
	: CCamera{ Prototype }
{
}

HRESULT CFPSCamera::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CFPSCamera::Initialize(void* pArg)
{
	CAMERA_FPS_DESC* pDesc = static_cast<CAMERA_FPS_DESC*>(pArg);


	if (FAILED(__super::Initialize(pDesc)))
		return E_FAIL;

	return S_OK;
}

void CFPSCamera::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);
}

void CFPSCamera::Update(_float fTimeDelta)
{
}

void CFPSCamera::Late_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);
}

HRESULT CFPSCamera::Render()
{
	return S_OK;
}

void CFPSCamera::SetMatrix(const _float4x4* pMatrix)
{
	m_pTransformCom->SetWorldMat(*pMatrix);
}

CFPSCamera* CFPSCamera::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CFPSCamera* pInstance = new CFPSCamera(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CFPSCamera"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



CGameObject* CFPSCamera::Clone(void* pArg)
{
	CFPSCamera* pInstance = new CFPSCamera(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CFPSCamera"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CFPSCamera::Free()
{
	__super::Free();

}
