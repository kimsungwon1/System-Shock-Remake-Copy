#include "stdafx.h"
#include "..\Public\Effect_GunFire.h"

#include "GameInstance.h"

CEffect_GunFire::CEffect_GunFire(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CBlendObject{ pDevice, pContext }
{
}

CEffect_GunFire::CEffect_GunFire(const CEffect_GunFire& Prototype)
	: CBlendObject{ Prototype }
{
}

HRESULT CEffect_GunFire::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CEffect_GunFire::Initialize(void* pArg)
{
	/* 직교퉁여을 위한 데이터들을 모두 셋하낟. */
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	auto* pDesc = static_cast<GUNFIRE_DESC*>(pArg);

	m_pOriginWorldTransform = pDesc->pWorldMatrixOrigin;
	m_fOffsetToOrigin.x = pDesc->fOffsets[0];
	m_fOffsetToOrigin.y = pDesc->fOffsets[1];
	m_fOffsetToOrigin.z = pDesc->fOffsets[2];

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_pTransformCom->Set_Scaled(0.1f, 0.1f, 1.f);
	m_fTurnAngle = pDesc->fAngle;

	return S_OK;
}

void CEffect_GunFire::Priority_Update(_float fTimeDelta)
{
	m_fFrame.x += fTimeDelta;
}

void CEffect_GunFire::Update(_float fTimeDelta)
{
	if (m_fFrame.x >= m_fFrame.y) {
		m_pGameInstance->Reserve_ToDelete(LEVEL_GAMEPLAY, TEXT("Layer_Effect"), this);
	}
}

void CEffect_GunFire::Late_Update(_float fTimeDelta)
{
	SetToSight();
	SetShirinking();
	/* 직교투영을 위한 월드행렬까지 셋팅하게 된다. */
	__super::Late_Update(fTimeDelta);

	m_pGameInstance->Add_RenderObject(CRenderer::RG_BLEND, this);
}

void CEffect_GunFire::SetShirinking()
{
	auto vScale = m_pTransformCom->Get_Scaled();
	m_pTransformCom->Set_Scaled(vScale.x * (m_fFrame.y - m_fFrame.x) / m_fFrame.y, vScale.y * (m_fFrame.y - m_fFrame.x) / m_fFrame.y, 1.f);

	m_pTransformCom->Turn(m_pTransformCom->Get_State(CTransform::STATE_LOOK), 1 / m_pTransformCom->GetRotationPerSec() * m_fTurnAngle);

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_pTransformCom->Get_State(CTransform::STATE_POSITION) - XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_UP)) * m_fFrame.x);
}

void CEffect_GunFire::SetToSight()
{
	_vector vPosition = XMLoadFloat3((_float3*)m_pOriginWorldTransform->m[3]);

	vPosition += m_fOffsetToOrigin.x * XMLoadFloat3((_float3*)m_pOriginWorldTransform->m[0]);
	vPosition += m_fOffsetToOrigin.y * XMLoadFloat3((_float3*)m_pOriginWorldTransform->m[1]);
	vPosition += m_fOffsetToOrigin.z * XMLoadFloat3((_float3*)m_pOriginWorldTransform->m[2]);

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPosition);
	auto* pPlayer = m_pGameInstance->Get_CloneObject_ByIndex(LEVEL_GAMEPLAY, TEXT("Layer_Player"));
	m_pTransformCom->SetLookToRightUp(pPlayer->GetTransformCom()->Get_State(CTransform::STATE_LOOK));
}

HRESULT CEffect_GunFire::Render()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;
	if (FAILED(m_pTextureCom->Bind_ShadeResource(m_pShaderCom, "g_Texture", 0)))
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

HRESULT CEffect_GunFire::Ready_Components()
{
	/* FOR.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPosTex"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* FOR.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_GunFire"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	/* FOR.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	return S_OK;
}

CEffect_GunFire* CEffect_GunFire::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CEffect_GunFire* pInstance = new CEffect_GunFire(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CEffect_GunFire"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



CGameObject* CEffect_GunFire::Clone(void* pArg)
{
	CEffect_GunFire* pInstance = new CEffect_GunFire(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CEffect_GunFire"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEffect_GunFire::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
}