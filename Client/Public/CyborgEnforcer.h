#pragma once
#include "UnitObject.h"


BEGIN(Client)

class CCyborgEnforcer : public CUnitObject
{
	enum Anim_State
	{
		Death_h,
		Death_p,
		Idle,
		Melee,
		Run_front,
		Walk_back,
		Walk_front,
		Walk_left,
		Walk_right,
		Range,
		Anim_END
	};
	enum ENFORCER_STATE {
		STATE_RESET = 0x00000000,
		STATE_STAND = 0x00000001,
		STATE_WALK = 0x00000002,
		STATE_TURN = 0x00000004,
		STATE_ATTACK = 0x00000008,
		STATE_END = 0xffffffff
	};

private:
	CCyborgEnforcer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCyborgEnforcer(const CUnitObject& Prototype);
	virtual ~CCyborgEnforcer() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	_bool setIdle(_float fTimeDelta) override;
	_bool setDead(_float fTimeDelta) override;

	virtual _bool move_By_AStar(_float fTimeDelta) override;

	virtual _bool find_Player(_float fTimeDelta) override;

	virtual _bool turn_To_Player(_float fTimeDelta) override;

	_bool shoot_Player(_float fTimeDelta);

	virtual _bool attack_normal(_float fTimeDelta) override;
	virtual void find_path() override;

private:
	Anim_State m_eAnimState = Idle;
	_uint m_iActionState = STATE_STAND;

	_float2 m_vShootCoolTime = { 0.f, 0.8f };
public:
	static CCyborgEnforcer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;

};

END