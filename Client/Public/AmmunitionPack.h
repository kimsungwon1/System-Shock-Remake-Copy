#pragma once
#include "Item.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CShader;
class CModel;
class CCollider;
END

BEGIN(Client)

class CAmmunitionPack : public CItem
{
public:
	struct AMMUNITIONPACK_DESC : CGameObject::GAMEOBJECT_DESC {
		_int iAmmunitionSize = 1;
	};
protected:
	CAmmunitionPack(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CAmmunitionPack(const CAmmunitionPack& Prototype);
	virtual ~CAmmunitionPack() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Priority_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Render();

	void JoinTogether(CAmmunitionPack* pOther);
	_int Get_AmmunitionSize() const { return  m_iAmmunitionSize; }
	void PopAmmu() { m_iAmmunitionSize--; }
protected:
	_int m_iAmmunitionSize = 1;
public:
	virtual CGameObject* Clone(void* pArg) = 0;
	virtual void Free() override;
};

END