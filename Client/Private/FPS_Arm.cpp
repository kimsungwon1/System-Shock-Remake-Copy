#include "stdafx.h"
#include "FPS_Arm.h"
#include "FPSPlayer.h"
#include "Model.h"
#include "Shader.h"
#include "GameInstance.h"

CFPS_Arm::CFPS_Arm(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPartObject(pDevice, pContext)
{
}

CFPS_Arm::CFPS_Arm(const CFPS_Arm& Prototype)
	: CPartObject(Prototype)
{
}

const _float4x4* CFPS_Arm::Get_BoneMatrix_Ptr(const _char* pBoneName) const
{
	return m_pModelCom->Get_BoneCombinedTransformMatrix_Ptr(pBoneName);
}

HRESULT CFPS_Arm::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CFPS_Arm::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg))) {
		return E_FAIL;
	}

	if (FAILED(Ready_Components())) {
		return E_FAIL;
	}

	m_pModelCom->SetUp_Animation(0, true);

	_matrix worldMat = XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrix_Ptr());

	worldMat.r[3] = worldMat.r[3] - XMVectorSet(0.15f, 0.2f, 0.f, 0.f);

	_float4x4 mat = {};
	XMStoreFloat4x4(&mat, worldMat);
	m_pTransformCom->SetWorldMat(mat);

	return S_OK;
}

void CFPS_Arm::Priority_Update(_float fTimeDelta)
{
}

void CFPS_Arm::Update(_float fTimeDelta)
{
	_float4x4 rootTransmat = {};
	if(!m_bPlayAnimNothing)
	{
		m_bAnimFinished = m_pModelCom->Play_Animation(fTimeDelta * 2, &rootTransmat);
	}
}

void CFPS_Arm::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);

	_matrix parentMatrix = XMLoadFloat4x4(m_pParentTransform);
	parentMatrix.r[3].m128_f32[1] += CFPSPlayer::Get_Height();
	_matrix worldMatrix = XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrix_Ptr()) * parentMatrix;

	XMStoreFloat4x4(&m_WorldMatrix, worldMatrix);

	if(m_bRenderOn)
		m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);
}

HRESULT CFPS_Arm::Render()
{
	if (FAILED(__super::Bind_WorldMatrix(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		if (m_pModelCom->GetType() == CModel::TYPE_ANIM)
		{
			m_pModelCom->Bind_MeshBoneMatrices(m_pShaderCom, "g_BoneMatrices", i);
		}
		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", aiTextureType_DIFFUSE, i)))
			return E_FAIL;
		_int iShader = 1;
		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_NormalTexture", aiTextureType_NORMALS, i)))
			iShader = 0;

		if (FAILED(m_pShaderCom->Begin(iShader)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render(i)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CFPS_Arm::Ready_Components()
{
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxAnimModel"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* FOR.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_PlayerHand"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	return S_OK;
}

vector<string> CFPS_Arm::Get_AnimationNames() const
{
	return m_pModelCom->GetAnimationNames();
}

vector<string> CFPS_Arm::Get_AnimationNames_by50(_uint iOffset) const
{
	return m_pModelCom->GetAnimationNames_By50(iOffset);
}

vector<string> CFPS_Arm::Get_AnimationNames_10(_uint iNum) const
{
	return m_pModelCom->GetAnimationNames_10(iNum);
}

size_t CFPS_Arm::Get_NumAnimation() const
{
	return m_pModelCom->GetNumAnimation();
}

_uint CFPS_Arm::Get_iCurrentAnimIndex() const
{
	return m_pModelCom->GetCurrentAnimIndex();
}

_double CFPS_Arm::Get_CurrentAnimDuration() const
{
	return m_pModelCom->GetCurrentAnimDuration();
}

void CFPS_Arm::SetUp_Animation(_uint iAnimIndex, _bool bLoop)
{
	m_bPlayAnimNothing = false;
	m_pModelCom->SetUp_Animation(iAnimIndex, bLoop);
}

void CFPS_Arm::SetUp_Animation_Immediately(_uint iAnimIndex, _bool bLoop)
{
	m_bPlayAnimNothing = false;
	m_pModelCom->SetUp_Animation_Immediately(iAnimIndex, bLoop);
}

void CFPS_Arm::Set_Order(_int iOrder)
{
	SetUp_Animation((_uint)iOrder);
}

_int CFPS_Arm::GetInfo(const _int* arrIndices) const
{
	_int infoNum = 1;
	return 1;
}

CFPS_Arm* CFPS_Arm::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CFPS_Arm* pInstance = new CFPS_Arm(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CFPS_Arm"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CFPS_Arm::Clone(void* pArg)
{
	CFPS_Arm* pInstance = new CFPS_Arm(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CFPS_Arm"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


void CFPS_Arm::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
}

