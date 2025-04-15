#include "..\Public\GameInstance.h"
#include "Graphic_Device.h"
#include "Level_Manager.h"
#include "Object_Manager.h"
#include "Timer_Manager.h"
#include "Input_Device.h"
#include "Raycaster.h"
#include "Light_Manager.h"
#include "Collider_Manager.h"
#include "Target_Manager.h"
#include "Pixel_Picking.h"
#include "Font_Manager.h"
#include "Frustum.h"

IMPLEMENT_SINGLETON(CGameInstance)

CGameInstance::CGameInstance()
{

}

HRESULT CGameInstance::Initialize_Engine(HINSTANCE hInst, _uint iNumLevels, const ENGINE_DESC& EngineDesc, _Inout_ ID3D11Device** ppDevice, _Inout_ ID3D11DeviceContext** ppContext)
{
	/* 그래픽 카드를 초기화하낟. */
	m_pGraphic_Device = CGraphic_Device::Create(EngineDesc.hWnd, EngineDesc.isWindowsed, EngineDesc.iWinSizeX, EngineDesc.iWinSizeY, ppDevice, ppContext);
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;

	m_pFrustum = CFrustum::Create();
	if (nullptr == m_pFrustum)
		return E_FAIL;

	m_pFont_Manager = CFont_Manager::Create(*ppDevice, *ppContext);
	if (nullptr == m_pFont_Manager)
		return E_FAIL;

	m_pPixel_Picking = CPixel_Picking::Create(*ppDevice, *ppContext, EngineDesc.hWnd);
	if (nullptr == m_pPixel_Picking)
		return E_FAIL;

	m_pTimer_Manager = CTimer_Manager::Create();
	if (nullptr == m_pTimer_Manager)
		return E_FAIL;

	m_pTarget_Manager = CTarget_Manager::Create(*ppDevice, *ppContext);
	if (nullptr == m_pTarget_Manager)
		return E_FAIL;

	m_pRenderer = CRenderer::Create(*ppDevice, *ppContext);
	if (nullptr == m_pRenderer)
		return E_FAIL;

	m_pPipeLine = CPipeLine::Create();
	if (nullptr == m_pPipeLine)
		return E_FAIL;

	m_pLight_Manager = CLight_Manager::Create();
	if (nullptr == m_pLight_Manager)
		return E_FAIL;

	///* 사운드 카드를 초기화하낟. */

	///* 입력장치를 초기화하낟. */
	m_pInput_Device = CInput_Device::Create(hInst, EngineDesc.hWnd);
	if (nullptr == m_pInput_Device)
		return E_FAIL;

	////m_pPicking = CPicking::Create(*ppDevice, EngineDesc.hWnd, EngineDesc.iWinSizeX, EngineDesc.iWinSizeY);
	////if (nullptr == m_pPicking)
	////	return E_FAIL;

	/* 여러가지 매니져를 초기화하낟. */
	m_pLevel_Manager = CLevel_Manager::Create();
	if (nullptr == m_pLevel_Manager)
		return E_FAIL;

	m_pObject_Manager = CObject_Manager::Create(iNumLevels);
	if (nullptr == m_pObject_Manager)
		return E_FAIL;

	m_pComponent_Manager = CComponent_Manager::Create(iNumLevels);
	if (nullptr == m_pComponent_Manager)
		return E_FAIL;

	m_pRaycaster = CRaycaster::Create(EngineDesc.hWnd, EngineDesc.iWinSizeX, EngineDesc.iWinSizeY);
	if (nullptr == m_pRaycaster)
		return E_FAIL;

	m_pCollider_Manager = CCollider_Manager::Create();
	if (nullptr == m_pCollider_Manager)
		return E_FAIL;

	return S_OK;
}



void CGameInstance::Update_Engine(_float fTimeDelta)
{
	/* 현재 키보드와 마우스의 상태를 받아올꺼야. */
	m_pInput_Device->Update();

	m_pObject_Manager->Priority_Update(fTimeDelta);	

	m_pPipeLine->Update();

	m_pFrustum->Update();

	m_pRaycaster->Update();

	m_pObject_Manager->Update(fTimeDelta);
	
	m_pObject_Manager->Late_Update(fTimeDelta);
	
	m_pLevel_Manager->Update(fTimeDelta);	

	if(m_iReservedLevelToChange != -1)
	{
		m_pLevel_Manager->Load_SubLevel(m_iReservedLevelToChange, XMLoadFloat3(&m_vPlayerNextPosition));
	}
	m_iReservedLevelToChange = -1;
}

HRESULT CGameInstance::Draw_Engine()
{
	
	m_pRenderer->Draw();

	return m_pLevel_Manager->Render();
}

HRESULT CGameInstance::Clear(_uint iLevelIndex)
{
	/* 특정 레벨을 위해 존재하는 자원들은 해당 레벨이 끝나면 삭제하자. */
	m_pObject_Manager->Clear(iLevelIndex);

	/* 컴포넌트 원형들도 레벨별로 관리했었다. */
	// m_pComponent_Manager->Clear(iLevelIndex);

	return S_OK;
}

ID3D11ShaderResourceView* CGameInstance::Get_BackBuffer_SRV() const
{
	return m_pGraphic_Device->Get_BackBuffer_SRV();
}

void CGameInstance::Render_Begin()
{
	/*m_pGraphic_Device->Render_Begin();*/
	m_pGraphic_Device->Clear_BackBuffer_View(_float4(1.f, 0.f, 1.f, 1.f));
	m_pGraphic_Device->Clear_DepthStencil_View();
	

}

void CGameInstance::Render_End()
{
	/*m_pGraphic_Device->Render_End(hWnd);*/

	m_pGraphic_Device->Present();
	
	
}
#pragma region INPUT_DEVICE
_byte CGameInstance::Get_DIKeyState(_ubyte byKeyID)
{
	return m_pInput_Device->Get_DIKeyState(byKeyID);	
}

_byte CGameInstance::Get_DIKeyStateDown(_ubyte byKeyID)
{
	return m_pInput_Device->Get_DIKeyStateDown(byKeyID);
}

_byte CGameInstance::Get_DIMouseState(MOUSEKEYSTATE eMouse)
{
	return m_pInput_Device->Get_DIMouseState(eMouse);
}

_byte CGameInstance::Get_DIMouseStateDown(MOUSEKEYSTATE eMouse)
{
	return m_pInput_Device->Get_DIMouseStateDown(eMouse);
}

_long CGameInstance::Get_DIMouseMove(MOUSEMOVESTATE eMouseState)
{
	return m_pInput_Device->Get_DIMouseMove(eMouseState);
}

_bool CGameInstance::Get_RealDIKeyStateDown(_ubyte byKeyID)
{
	return m_pInput_Device->Get_RealDIKeyStateDown(byKeyID);
}

_bool CGameInstance::Get_RealDIMouseStateDown(MOUSEKEYSTATE eMouse)
{
	return m_pInput_Device->Get_RealDIMouseStateDown(eMouse);
}

void CGameInstance::Show_Cursor(_bool bShow)
{
	m_pInput_Device->Show_Cursor(bShow);
}

_bool CGameInstance::IsShowing_Cursor() const
{
	return m_pInput_Device->IsShowing_Cursor();
}

#pragma endregion

#pragma region LEVEL_MANAGER

HRESULT CGameInstance::Change_Level(_uint iLevelIndex, CLevel * pNextLevel)
{
	return m_pLevel_Manager->Change_Level(iLevelIndex, pNextLevel);	
}

#pragma endregion

#pragma region OBJECT_MANAGER

HRESULT CGameInstance::Add_Prototype(const _wstring & strPrototypeTag, CGameObject * pPrototype)
{
	return m_pObject_Manager->Add_Prototype(strPrototypeTag, pPrototype);
}


HRESULT CGameInstance::Add_CloneObject_ToLayer(_uint iLevelIndex, const _wstring & strLayerTag, const _wstring & strPrototypeTag, void * pArg)
{
	return m_pObject_Manager->Add_CloneObject_ToLayer(iLevelIndex, strLayerTag, strPrototypeTag, pArg);
}

HRESULT CGameInstance::Delete_CloneObject_ByPointer(_uint iLevelIndex, const _wstring& strLayerTag, const CGameObject* pObject)
{
	return m_pObject_Manager->Delete_CloneObject_ByPointer(iLevelIndex, strLayerTag, pObject);
}

CGameObject* CGameInstance::Get_CloneObject_ByIndex(_uint iLevelIndex, const _wstring& strLayerTag, _int iIndex)
{
	return m_pObject_Manager->Get_CloneObject_ByIndex(iLevelIndex, strLayerTag, iIndex);
}

CComponent * CGameInstance::Find_Component(_uint iLevelIndex, const _wstring & strLayerTag, const _wstring & strComponentTag, _uint iIndex)
{
	return m_pObject_Manager->Find_Component(iLevelIndex, strLayerTag, strComponentTag, iIndex);	
}

CGameObject* CGameInstance::Clone_Object(const _wstring& strProtototypeTag, void* pArg)
{
	return m_pObject_Manager->Clone_Object(strProtototypeTag, pArg);
}

list<class CGameObject*>* CGameInstance::Get_RefGameObjects(_uint iLevelIndex, const _wstring& strLayerTag)
{
	return m_pObject_Manager->GetRefGameObjects(iLevelIndex, strLayerTag);
}

HRESULT CGameInstance::Reserve_ToDelete(_uint iLevelIndex, const _wstring& strLayerTag, const CGameObject* pObject)
{
	return m_pObject_Manager->Reserve_ToDelete(iLevelIndex, strLayerTag, pObject);
}

void CGameInstance::Reserve_Timealter(_float fTimeAlterRatio, _float fTimeAlterRatio_ToException, _float fTimeAlterLast, const _wstring& strExceptionLayer)
{
	m_pObject_Manager->Reserve_Timealter(fTimeAlterRatio, fTimeAlterRatio_ToException, fTimeAlterLast, strExceptionLayer);
}

_bool CGameInstance::IsTimeSlowing()
{
	return m_pObject_Manager->IsTimeSlowing();
}

HRESULT CGameInstance::Add_Octree(_uint iLevelIndex, const _wstring& strLayerTag, _float iMinRectlength)
{
	return m_pObject_Manager->Add_Octree(iLevelIndex, strLayerTag, iMinRectlength);
}

HRESULT CGameInstance::Delete_Octree(_uint iLevelIndex, const _wstring& strLayerTag)
{
	return m_pObject_Manager->Delete_Octree(iLevelIndex, strLayerTag);
}

//void CGameInstance::Push_Back_ToOctree(_uint iLevelIndex, const _wstring& strLayerTag, const CGameObject* pVar, _fvector vVarCenter)
//{
//	m_pObject_Manager->Push_Back(iLevelIndex, strLayerTag, pVar, vVarCenter);
//}

list<CGameObject*> CGameInstance::GetList_AtOctree_ByRadius(_uint iLevelIndex, const _wstring& strLayerTag, _fvector vCenter, _float fRadius)
{
	return m_pObject_Manager->GetList_ByRadius(iLevelIndex, strLayerTag, vCenter, fRadius);
}

#pragma endregion

#pragma region COMPONENT_MANAGER

HRESULT CGameInstance::Add_Prototype(_uint iLevelIndex, const _wstring & strPrototypeTag, CComponent * pPrototype)
{
	return m_pComponent_Manager->Add_Prototype(iLevelIndex, strPrototypeTag, pPrototype);	
}

CComponent * CGameInstance::Clone_Component(_uint iLevelIndex, const _wstring & strPrototypeTag, void * pArg)
{
	return m_pComponent_Manager->Clone_Component(iLevelIndex, strPrototypeTag, pArg);
}

#pragma endregion

#pragma region TIMER_MANAGER

HRESULT CGameInstance::Add_Timer(const _wstring & strTimerTag)
{
	return m_pTimer_Manager->Add_Timer(strTimerTag);
}

_float CGameInstance::Compute_TimeDelta(const _wstring & strTimerTag)
{
	return m_pTimer_Manager->Compute_TimeDelta(strTimerTag);
}

#pragma endregion


#pragma region RENDERER

HRESULT CGameInstance::Add_RenderObject(CRenderer::RENDERGROUP eRenderGroupID, CGameObject * pRenderObject)
{
	return m_pRenderer->Add_RenderObject(eRenderGroupID, pRenderObject);
}

void CGameInstance::SetBlur(_bool bBlur)
{
	m_pRenderer->SetBlur(bBlur);
}

#ifdef _DEBUG
HRESULT CGameInstance::Add_DebugObject(CComponent* pDebugObject)
{
	return m_pRenderer->Add_DebugObject(pDebugObject);
}
#endif

#pragma endregion

#pragma region PIPELINE
void CGameInstance::Set_Transform(CPipeLine::D3DTRANSFORMSTATE eState, _fmatrix TransformMatrix)
{
	return m_pPipeLine->Set_Transform(eState, TransformMatrix);
}
_float4x4 CGameInstance::Get_Transform_Float4x4(CPipeLine::D3DTRANSFORMSTATE eState) const
{
	return m_pPipeLine->Get_Transform_Float4x4(eState);
}
_matrix CGameInstance::Get_Transform_Matrix(CPipeLine::D3DTRANSFORMSTATE eState) const
{
	return m_pPipeLine->Get_Transform_Matrix(eState);
}
_float4x4 CGameInstance::Get_Transform_Inverse_Float4x4(CPipeLine::D3DTRANSFORMSTATE eState) const
{
	return m_pPipeLine->Get_Transform_Inverse_Float4x4(eState);
}
_matrix CGameInstance::Get_Transform_Inverse_Matrix(CPipeLine::D3DTRANSFORMSTATE eState) const
{
	return m_pPipeLine->Get_Transform_Inverse_Matrix(eState);
}

_float4 CGameInstance::Get_CamPosition_Float4() const
{
	return m_pPipeLine->Get_CamPosition_Float4();
}
_vector CGameInstance::Get_CamPosition_Vector() const
{
	return m_pPipeLine->Get_CamPosition_Vector();
}

HRESULT CGameInstance::Add_Light(const LIGHT_DESC& LightDesc, _int iIndex)
{
	return m_pLight_Manager->Add_Light(LightDesc, iIndex);
}

HRESULT CGameInstance::Change_Light(const LIGHT_DESC& LightDesc, _int iIndex)
{
	return m_pLight_Manager->Change_Light(LightDesc, iIndex);
}

HRESULT CGameInstance::Delete_Light(_int iIndex)
{
	if (m_pLight_Manager == nullptr) {
		return E_FAIL;
	}
	return m_pLight_Manager->Delete_Light(iIndex);
}

const LIGHT_DESC* CGameInstance::Get_LightDesc(_uint iIndex) const
{
	return m_pLight_Manager->Get_LightDesc(iIndex);
}

HRESULT CGameInstance::Render_Lights(CShader* pShader, CVIBuffer_Rect* pVIBuffer)
{
	return m_pLight_Manager->Render(pShader, pVIBuffer);
}

HRESULT CGameInstance::Add_RenderTarget(const _wstring& strTargetTag, _uint iWidth, _uint iHeight, DXGI_FORMAT ePixelFormat, const _float4& vClearColor)
{
	return m_pTarget_Manager->Add_RenderTarget(strTargetTag, iWidth, iHeight, ePixelFormat, vClearColor);
}

HRESULT CGameInstance::Add_MRT(const _wstring& strMRTTag, const _wstring& strTargetTag)
{
	return m_pTarget_Manager->Add_MRT(strMRTTag, strTargetTag);
}

HRESULT CGameInstance::Begin_MRT(const _wstring& strMRTTag)
{
	return m_pTarget_Manager->Begin_MRT(strMRTTag);
}

HRESULT CGameInstance::End_MRT()
{
	return m_pTarget_Manager->End_MRT();
}

HRESULT CGameInstance::Bind_RT_ShaderResource(CShader* pShader, const _wstring& strTargetTag, const _char* pContantName)
{
	return m_pTarget_Manager->Bind_ShaderResource(pShader, strTargetTag, pContantName);
}

#ifdef _DEBUG
HRESULT CGameInstance::Ready_RT_Debug(const _wstring& strTargetTag, _float fX, _float fY, _float fSizeX, _float fSizeY)
{
	return m_pTarget_Manager->Ready_Debug(strTargetTag, fX, fY, fSizeX, fSizeY);
}

HRESULT CGameInstance::Render_MRT_Debug(const _wstring& strMRTTag, CShader* pShader, CVIBuffer_Rect* pVIBuffer)
{
	return m_pTarget_Manager->Render(strMRTTag, pShader, pVIBuffer);
}
void CGameInstance::SetDebugRenderOnOff()
{
	m_pRenderer->SetDebugRenderOnOff();
}
#endif

HRESULT CGameInstance::Copy_RenderTarget(const _wstring& strTargetTag, ID3D11Texture2D* pTexture)
{
	return m_pTarget_Manager->Copy_RenderTarget(strTargetTag, pTexture);
}

_bool CGameInstance::Pixel_Picking(_float3* pPickPos)
{
	return m_pPixel_Picking->Picking(pPickPos);
}

void CGameInstance::Transform_MouseRay_ToLocalSpace(const _float4x4& WorldMatrix)
{
	m_pRaycaster->Transform_ToLocalSpace(WorldMatrix);
}

_bool CGameInstance::isPicked_InLocalSpace(_vector vPointA, _vector vPointB, _vector vPointC, _float3* pOut)
{
	return m_pRaycaster->isPicked_InLocalSpace(vPointA, vPointB, vPointC, pOut);
}

_bool CGameInstance::isPicked_InWorldSpace(_vector vPointA, _vector vPointB, _vector vPointC, _float3* pOut)
{
	return m_pRaycaster->isPicked_InWorldSpace(vPointA, vPointB, vPointC, pOut);
}

void CGameInstance::Set_RayCaster_Center()
{
	m_pRaycaster->Set_RayCaster_Center();
}

void CGameInstance::Set_RayCaster_Mouse()
{
	m_pRaycaster->Set_RayCaster_Mouse();
}

HRESULT CGameInstance::Add_Collider(CCollider* pCollider)
{
	return m_pCollider_Manager->Add_Collider(pCollider);
}

HRESULT CGameInstance::Delete_Collider(CCollider* pCollider)
{
	if (m_pCollider_Manager == nullptr) {
		return S_OK;
	}
	return m_pCollider_Manager->Delete_Collider(pCollider);
}

_bool CGameInstance::Intersects(CCollider* pCollider, _float3* pColliderVec)
{
	return m_pCollider_Manager->Intersects(pCollider, pColliderVec);
}

HRESULT CGameInstance::Add_Font(const _wstring& strFontTag, const _tchar* pFontFilePath)
{
	return m_pFont_Manager->Add_Font(strFontTag, pFontFilePath);
}

HRESULT CGameInstance::Render_Text(const _wstring& strFontTag, const _tchar* pText, _fvector vPosition, _fvector vColor, _float fRadian, _fvector vPivot, _float fScale)
{
	return m_pFont_Manager->Render(strFontTag, pText, vPosition, vColor, fRadian, vPivot, fScale);
}

_bool CGameInstance::IsIn_Frustum_WorldSpace(_fvector vPosition, _float fRadius)
{
	return m_pFrustum->IsIn_WorldSpace(vPosition, fRadius);
}

#pragma endregion

void CGameInstance::Release_Engine()
{	
	Safe_Release(m_pFrustum);
	Safe_Release(m_pFont_Manager);
	Safe_Release(m_pPixel_Picking);
	Safe_Release(m_pTarget_Manager);
	Safe_Release(m_pCollider_Manager);
	Safe_Release(m_pLight_Manager);
	Safe_Release(m_pRaycaster);
	Safe_Release(m_pPipeLine);
	Safe_Release(m_pRenderer);
	Safe_Release(m_pTimer_Manager);
	Safe_Release(m_pComponent_Manager);
	Safe_Release(m_pObject_Manager);
	Safe_Release(m_pLevel_Manager);
	Safe_Release(m_pInput_Device);
	Safe_Release(m_pGraphic_Device);

	CGameInstance::Get_Instance()->Destroy_Instance();	
}

void CGameInstance::Free()
{	
	__super::Free();

}
