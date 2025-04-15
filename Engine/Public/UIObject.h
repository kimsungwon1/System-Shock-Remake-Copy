#pragma once

#include "GameObject.h"

/* ���������� �ʿ��� �ټ��� ��ü�� ������ �� �ִ�.  */
/* ���������� �ʿ��� �����͵�� ����� �������ִ� ��Ȱ */
BEGIN(Engine)

class ENGINE_DLL CUIObject abstract : public CGameObject
{
public:
	typedef struct : public CGameObject::GAMEOBJECT_DESC
	{
		_float	fX{}, fY{}, fSizeX{}, fSizeY{};
		CUIObject* pParent = nullptr;
	} UI_DESC;

	// �ڽ� ������ ���� �θ�� ���� Ÿ���� ������. ��� �θ�� �Բ� �Ҹ��� �� �ִ�.
	enum UI_TYPE {
		TYPE_HEALTHBAR = 1,
		TYPE_ELEVATOR,
		TYPE_BULLETS,
		TYPE_CROSSHAIR,
		TYPE_END
	};

protected:
	CUIObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUIObject(const CUIObject& Prototype);
	virtual ~CUIObject() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

protected:
	_float				m_fX{}, m_fY{}, m_fSizeX{}, m_fSizeY{};
	_float4x4			m_ViewMatrix{}, m_ProjMatrix{};
	_uint				m_iDepth = {};

	_float				m_fViewWidth{}, m_fViewHeight{};

	_int m_iRenderPass = 0;
protected:
	class CShader* m_pShaderCom = { nullptr };
	class CTexture* m_pTextureCom = { nullptr };
	class CVIBuffer_Rect* m_pVIBufferCom = { nullptr };

protected:
	virtual HRESULT Ready_PartUI() { return S_OK; }
	HRESULT Add_PartUI(_uint iPartID, const _wstring& strPrototypeTag, void* pArg);
	vector<CUIObject*> m_vecPartUIs = {};
protected:
	CUIObject* m_pParentUI = nullptr;
	_bool m_bMouseOn = false;
	UI_TYPE m_eType = {};
public:
	_bool IsMouseOn() const { return m_bMouseOn; }
	_float2 Get_UIPos() const { return { m_fX, m_fY }; }
	void Set_UIPos(_float2 vPos) { m_fX = vPos.x; m_fY = vPos.y; }
public:
	virtual CGameObject* Clone(void* pArg) = 0;
	virtual void Free() override;
};

END