#include "stdafx.h"
#include "Effect_BulletCrater.h"

#include "GameInstance.h"

CEffect_BulletCrater::CEffect_BulletCrater(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CBlendObject{ pDevice, pContext }
{
}

CEffect_BulletCrater::CEffect_BulletCrater(const CEffect_BulletCrater& Prototype)
	: CBlendObject{ Prototype }
{
}

HRESULT CEffect_BulletCrater::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CEffect_BulletCrater::Initialize(void* pArg)
{
	/* 직교퉁여을 위한 데이터들을 모두 셋하낟. */
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	//m_pTransformCom->Set_Scaled(3.f, 3.f, 3.f);

	return S_OK;
}

void CEffect_BulletCrater::Priority_Update(_float fTimeDelta)
{
	//m_fFrame.x += fTimeDelta;
}

void CEffect_BulletCrater::Update(_float fTimeDelta)
{
	/*auto* pPlayer = m_pGameInstance->Get_CloneObject_ByIndex(LEVEL_GAMEPLAY, TEXT("Layer_Player"));
	m_pTransformCom->SetLookToRightUp(pPlayer->GetTransformCom()->Get_State(CTransform::STATE_LOOK));*/

	/*if (m_fFrame.x >= m_fFrame.y) {
		m_pGameInstance->Reserve_ToDelete(LEVEL_GAMEPLAY, TEXT("Layer_Effect"), this);
	}*/
}

void CEffect_BulletCrater::Late_Update(_float fTimeDelta)
{
	/* 직교투영을 위한 월드행렬까지 셋팅하게 된다. */
	__super::Late_Update(fTimeDelta);

	m_pGameInstance->Add_RenderObject(CRenderer::RG_BLEND, this);
}


HRESULT CEffect_BulletCrater::Render()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;
	if (FAILED(m_pTextureCom->Bind_ShadeResource(m_pShaderCom, "g_Texture", 0)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(0)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;
	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CEffect_BulletCrater::Ready_Components()
{
	/* FOR.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPosTex"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* FOR.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_BulletCrater"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	/* FOR.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	return S_OK;
}

CEffect_BulletCrater* CEffect_BulletCrater::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CEffect_BulletCrater* pInstance = new CEffect_BulletCrater(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CEffect_BulletCrater"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CEffect_BulletCrater::Clone(void* pArg)
{
	CEffect_BulletCrater* pInstance = new CEffect_BulletCrater(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CEffect_BulletCrater"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEffect_BulletCrater::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
}