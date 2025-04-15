#pragma once
#include "GameObject.h"

BEGIN(Engine)

class ENGINE_DLL CPartObject : public CGameObject
{
public:
	struct PartObject_DESC : GAMEOBJECT_DESC{
		const _float4x4* pParentTransform = { nullptr };
	};

protected:
	CPartObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPartObject(const CPartObject& Prototype);
	virtual ~CPartObject() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Priority_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Render();

public:
	HRESULT Bind_WorldMatrix(class CShader* pShader, const char* constantName);
	virtual void Set_Order(_int iOrder) {}
protected:
	const _float4x4* m_pParentTransform = nullptr;
	_float4x4 m_WorldMatrix = {};

public:
	virtual CGameObject* Clone(void* pArg) = 0;
	virtual void Free() override;
};

END