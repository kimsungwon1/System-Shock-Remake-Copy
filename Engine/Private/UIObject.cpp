#include "..\Public\UIObject.h"
#include "GameInstance.h"


CUIObject::CUIObject(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject { pDevice, pContext }
{

}

CUIObject::CUIObject(const CUIObject & Prototype)
	: CGameObject{ Prototype }
{

}

HRESULT CUIObject::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUIObject::Initialize(void * pArg)
{
	UI_DESC*	pDesc = static_cast<UI_DESC*>(pArg);

	/* 유아이의 초기 위치와 사이즈를 설정한다. */
	m_fX = pDesc->fX;
	m_fY = pDesc->fY;
	m_fSizeX = pDesc->fSizeX;
	m_fSizeY = pDesc->fSizeY;	
	m_pParentUI = pDesc->pParent;

	_uint		iNumView = { 1 };
	D3D11_VIEWPORT		ViewportDesc{};

	m_pContext->RSGetViewports(&iNumView, &ViewportDesc);

	/* m_ViewMatrix : XMFLOAT4X4 */
	/* XMMatrixIdentity함수의 리턴값 XMMATRIX */
	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());			
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH(ViewportDesc.Width, ViewportDesc.Height, 0.f, 1.f));
	
	m_fViewWidth = ViewportDesc.Width;
	m_fViewHeight = ViewportDesc.Height;

	// XMVector3Equal(XMLoadFloat3(&m_vTmp), XMLoadFloat3(&m_vTmp));	

	if (FAILED(__super::Initialize(pDesc)))
		return E_FAIL;

	return S_OK;
}

void CUIObject::Priority_Update(_float fTimeDelta)
{

	for (auto* pUI : m_vecPartUIs) {
		if (pUI == nullptr) {
			continue;
		}
		pUI->Priority_Update(fTimeDelta);
	}
}

void CUIObject::Update(_float fTimeDelta)
{
	for (auto* pUI : m_vecPartUIs) {
		if (pUI == nullptr) {
			continue;
		}
		pUI->Update(fTimeDelta);
	}
}

void CUIObject::Late_Update(_float fTimeDelta)
{
	/* 직교투영을 위한 월드 행렬을 만들어 준다. */

	m_pTransformCom->Set_Scaled(m_fSizeX, m_fSizeY, 1.f);

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, 
		XMVectorSet(m_fX - m_fViewWidth * 0.5f, -m_fY + m_fViewHeight * 0.5f, 0.f, 1.f));
	m_pGameInstance->Add_RenderObject(CRenderer::RG_UI, this);

	for (auto* pUI : m_vecPartUIs) {
		if (pUI == nullptr) {
			continue;
		}
		pUI->Late_Update(fTimeDelta);
	}
}

HRESULT CUIObject::Render()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;
	if (FAILED(m_pTextureCom->Bind_ShadeResource(m_pShaderCom, "g_Texture", 0)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(m_iRenderPass)))
		return E_FAIL;


	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;
	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}


HRESULT CUIObject::Add_PartUI(_uint iPartID, const _wstring& strPrototypeTag, void* pArg)
{
	auto* pObject = m_pGameInstance->Clone_Object(strPrototypeTag, pArg);
	CUIObject* pUI = dynamic_cast<CUIObject*>(pObject);

	if (pUI == nullptr) {
		Safe_Release(pObject);
		return E_FAIL;
	}

	m_vecPartUIs[iPartID] = pUI;

	return S_OK;
}

void CUIObject::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);

	for (auto* pUI : m_vecPartUIs) {
		Safe_Release(pUI);
	}
}
