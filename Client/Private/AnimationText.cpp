#include "stdafx.h"
#include "AnimationText.h"
#include "GameInstance.h"
#include "FPSPlayer.h"
#include "FPS_Arm.h"


wstring StringToWString(const string& s) {
	wstring ret(s.begin(), s.end());

	return ret;
}

CAnimationText::CAnimationText(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CTextObject(pDevice, pContext)
{
}

CAnimationText::CAnimationText(const CAnimationText& Prototype)
    : CTextObject(Prototype)
{
}

HRESULT CAnimationText::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CAnimationText::Initialize(void* pArg)
{
    m_Pos = { 50, g_iWinSizeY * 0.5f };

    return S_OK;
}

void CAnimationText::Priority_Update(_float fTimeDelta)
{
	
}

void CAnimationText::Update(_float fTimeDelta)
{
	if(m_bControlAnim)
	{
		auto anims = m_pPlayerArm->Get_AnimationNames_10(m_iAnimationIndex);
		m_szText.clear();
		m_szText += StringToWString(anims[0]);

		for (size_t i = 1; i < anims.size(); i++) {
			m_szText += '\n';
			m_szText += StringToWString(anims[i]);
		}
	}
}

void CAnimationText::Late_Update(_float fTimeDelta)
{
	if(m_bControlAnim)
	{
		__super::Late_Update(fTimeDelta);
	}
}

HRESULT CAnimationText::Render()
{
    __super::Render();

    return S_OK;
}

void CAnimationText::AnimControl()
{
	if (m_pGameInstance->Get_RealDIKeyStateDown(DIKEYBOARD_F9)) {
		m_bControlAnim = !m_bControlAnim;
	}

	if (m_bControlAnim)
	{
		_long mouseWheelMove = m_pGameInstance->Get_DIMouseMove(MOUSEMOVESTATE::DIMM_WHEEL);

		if (mouseWheelMove < 0) {
			m_iAnimationIndex++;
		}
		else if (mouseWheelMove > 0) {
			m_iAnimationIndex--;
		}
		if (m_iAnimationIndex < 0) {
			m_iAnimationIndex = 0;
		}
		if (m_iAnimationIndex >= m_pPlayerArm->Get_NumAnimation()) {
			m_iAnimationIndex = m_pPlayerArm->Get_NumAnimation() - 1;
		}
		if (mouseWheelMove != 0)
			m_pPlayerArm->SetUp_Animation(m_iAnimationIndex, false);
	}
}

CAnimationText* CAnimationText::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CAnimationText* pInstance = new CAnimationText(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype())) {
		MSG_BOX(TEXT("Failed to Initialize : Create CAnimationText"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CAnimationText::Clone(void* pArg)
{
	CAnimationText* pInstance = new CAnimationText(*this);

	if (FAILED(pInstance->Initialize(pArg))) {
		MSG_BOX(TEXT("Failed to Initialize : Clone CAnimationText"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CAnimationText::Free()
{
	__super::Free();
}
