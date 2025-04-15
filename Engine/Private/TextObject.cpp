#include "TextObject.h"
#include "GameInstance.h"

CTextObject::CTextObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{
	m_pFont = new SpriteFont(pDevice, TEXT("../Bin/FontFiles/myfile.spritefont"));
	m_pBatch = new SpriteBatch(pContext);
}

CTextObject::CTextObject(const CTextObject& Prototype)
	: CGameObject{ Prototype }
	, m_pFont { Prototype.m_pFont }
	, m_pBatch { Prototype.m_pBatch }
{
	_int a = 10;
}

HRESULT CTextObject::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CTextObject::Initialize(void* pArg)
{
	TEXT_DESC* pDesc = static_cast<TEXT_DESC*>(pArg);

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	m_szText = pDesc->szText;

	m_Pos.x = pDesc->fPosX;
	m_Pos.y = pDesc->fPosY;

	return S_OK;
}

void CTextObject::Priority_Update(_float fTimeDelta)
{

}

void CTextObject::Update(_float fTimeDelta)
{
}

void CTextObject::Late_Update(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderObject(CRenderer::RG_UI, this);
}

HRESULT CTextObject::Render()
{
	m_pBatch->Begin();

	//auto origin = m_pFont->MeasureString(m_szText.c_str()) / 2.f;

	m_pFont->DrawString(m_pBatch, m_szText.c_str(),
		m_Pos, Colors::White, 0.f);

	m_pBatch->End();

	return S_OK;
}

CTextObject* CTextObject::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CTextObject* pInstance = new CTextObject(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype())) {
		MSG_BOX(TEXT("Failed to Initialize : Create CTextObject"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CTextObject::Clone(void* pArg)
{
	CTextObject* pInstance = new CTextObject(*this);

	if (FAILED(pInstance->Initialize(pArg))) {
		MSG_BOX(TEXT("Failed to Initialize : Clone CTextObject"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CTextObject::Free()
{
	__super::Free();

	if(!m_bCloned)
	{
		Safe_Delete(m_pFont);
		Safe_Delete(m_pBatch);
	}
}
