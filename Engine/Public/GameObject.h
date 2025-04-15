#pragma once

#include "Base.h"
#include "Transform.h"

/* 모든 게임내에 사용되는 게임 오브젝트들의 부모 클래스다. */

BEGIN(Engine)

class ENGINE_DLL CGameObject abstract : public CBase
{
public:
	struct GAMEOBJECT_DESC : public CTransform::TRANSFORM_DESC
	{
		GAMEOBJECT_DESC()
			: iCurrentLevel {0}
		{
			XMStoreFloat4x4(&transMat, XMMatrixIdentity());
		}
		XMFLOAT4X4 transMat;
		_int iCurrentLevel;
	} ;

protected:
	CGameObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGameObject(const CGameObject& Prototype);
	virtual ~CGameObject() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Priority_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Render();

public:
	class CComponent* Find_Component(const _wstring& strComponentTag);

public:
	CTransform* GetTransformCom() { return m_pTransformCom; }
	const CTransform* GetTransformCom() const { return m_pTransformCom; }
	virtual HRESULT SaveObject(ofstream& saveStream) const;
	virtual HRESULT LoadObject(ifstream& loadStream);
public:
	AllObjectType GetType() const { return m_eObjType; }
public:
	virtual _bool IsPicking(_float3 *pOut) { return false; }

	_bool IsDead() const { return m_bDead; }
	void SetRenderOnOff(_bool bRenderOn) { m_bRenderOn = bRenderOn; }
protected:
	ID3D11Device*				m_pDevice = { nullptr };
	ID3D11DeviceContext*		m_pContext = { nullptr };

	class CTransform*			m_pTransformCom = { nullptr };
	class CGameInstance*		m_pGameInstance = { nullptr };

protected:
	map<const _wstring, class CComponent*>			m_Components;
	AllObjectType m_eObjType = AllObjectType::TYPE_END;
	_bool m_bDead = false;
	_int m_iCurrentLevel = { 0 };
	_bool m_bRenderOn = true;
	
	_float m_fFrustumRadius = { 0 };
public:
	void SetLevel(_int iLevel) { m_iCurrentLevel = iLevel; }
	virtual _int GetInfo(const _int* arrIndices) const { return 0; }
protected:
	
	HRESULT Add_Component(_uint iLevelIndex, const _wstring& strPrototypeTag, const _wstring& strComponentTag, CComponent** ppOut, void* pArg = nullptr);

	_bool m_bCloned = false;
public:
	virtual CGameObject* Clone(void* pArg) = 0;
	virtual void Free() override;
};

END