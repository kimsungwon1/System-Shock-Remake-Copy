#pragma once
#include "GameObject.h"

BEGIN(Engine)

class ENGINE_DLL CContainObject abstract : public CGameObject
{
protected:
	CContainObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CContainObject(const CContainObject& Prototype);
	virtual ~CContainObject() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Priority_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Render();

protected:
	vector<class CPartObject*> m_vecParts;

protected:
	HRESULT Add_PartObject(_uint iPartID, const _wstring& strPrototypeTag, void* pArg = nullptr);

public:
	virtual void Free() override;
};

END
