#pragma once
#include "GameObject.h"

BEGIN(Engine)

class ENGINE_DLL CTextObject : public CGameObject
{
public:
	typedef struct : public CGameObject::GAMEOBJECT_DESC
	{
		_wstring szText = {};
		_float fPosX = {};
		_float fPosY = {};
	}TEXT_DESC;
protected:
	CTextObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTextObject(const CTextObject& Prototype);
	virtual ~CTextObject() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Priority_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Render();

protected:
	_wstring m_szText = {};
	_float2 m_Pos = {};

private:
	DirectX::SpriteBatch* m_pBatch = { nullptr };
	DirectX::SpriteFont* m_pFont = { nullptr };
public:
	static CTextObject* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END

