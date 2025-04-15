#pragma once
#include "UnitObject.h"
class CCyborgElite : public CUnitObject
{
public:
	enum Anim_State {
		Grenade_throw_from,
		Grenade_throw_realThrow,
		Attack_ranged,
		Attack_ranged_from,
		Attack_ranged_return,
		Death_h,
		Death_p,
		Hit,
		Idle,
		Run_back,
		Run_front,
		Run_left,
		Run_right,
		Turn_left_0,
		Turn_left_180,
		Turn_left_90,
		Turn_right_180,
		Turn_right_90,
		Walk_back,
		Walk_front,
		Walk_left,
		Walk_right,
		Teleport_intro,
		Attack_melee_a,
		Attack_melee_b,
		Melee_ready_from,
		Melee_ready_idle,
		Melee_ready_to_idle,
		Quickdash_l,
		Diego_Run,
		Run_Attack_Uppercut,

		ANIM_END
	};
	enum Elite_State {
		STATE_RESET = 0,
		STATE_STAND,
		STATE_WALK,
		STATE_TURN,
		STATE_TELEPORT_INTRO,
		STATE_ATTACK_MELEE,
		STATE_ATTACK_CHARGE,
		STATE_ATTACK_NORMAL_RANGE,
		STATE_ATTACK_GRENADE,	
		STATE_ATTACK_UPPERCUT,
		STATE_DODGE,

		STATE_END
	};

private:
	CCyborgElite(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCyborgElite(const CUnitObject& Prototype);
	virtual ~CCyborgElite() = default;

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

	virtual _bool turn_To_Player(_float fTimeDelta) override;

	_bool shoot_Player(_float fTimeDelta);
	void calculate_Grenade_Shooting();

	virtual _bool attack_normal(_float fTimeDelta) override;
	void attack_explosive(_float fTimeDelta); // 수류탄 던지기
	void attack_melee(_float fTimeDelta);
	void attack_melee_Uppercut(_float fTimeDelta);	// 어퍼컷- 플레이어가 맞으면 위로 날라감 ㅋㅋㅋ

	void CinematicTeleport(_float fTimeDelta);
	//void GrenadeThrow(_float fTimeDelta);
	void dodge(_float fTimeDelta);

private:
	Anim_State m_eAnimState = Idle;
	Elite_State m_eActionState = STATE_STAND;
	const _float m_fMeleeDistance = 3.f;
	_int	m_iExplosiveBlowed = 0;
	_float m_fDropTime = 0.f;
	_float2 m_vShootCoolTime = { 0.f, 1.f };
	_float2 m_vGrenadeCoolTime = { 0.f, 3.f };
	_float2 m_vUppercutCoolTime = { 0.f, 6.f };
public:
	static CCyborgElite* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;


};

