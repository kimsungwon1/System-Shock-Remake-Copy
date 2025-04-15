#pragma once
#include "UnitObject.h"

BEGIN(Client)

class CCortexReaver final : public CUnitObject
{
public:
	enum Anim_State {
		Melee_Attack,
		Death,
		DeathCinema,
		disable_loop,
		dropdown_1000u,
		explosive_attack_fire,
		explosive_attack_loop,
		explosive_attack_start,
		gas_grenade_attack_end,
		gas_grenade_attack_loop,
		gas_grenade_attack_idle,
		gas_grenade_attack_start,
		Idle,
		Run_Back,
		Run_Front,
		Run_Left,
		Run_Right,
		Dodge_Left,
		Dodge_Right,
		Turn_Left,
		Turn_Right,
		Walk_Back,
		Walk_Front,
		Walk_Left,
		Walk_Right,
		ANIM_END
	};
	enum CortexReaver_State {
		STATE_RESET = 0,
		STATE_STAND,
		STATE_WALK,
		STATE_TURN,
		STATE_DROP,
		STATE_ATTACK_MELEE,
		STATE_ATTACK_NORMAL_RANGE,
		STATE_ATTACK_EXPLOSIVE,
		STATE_ATTACK_LIGHTNING,
		STATE_ATTACK_LIGHTNING_SHORT,
		STATE_DODGE,

		STATE_END
	};
private:
	CCortexReaver(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCortexReaver(const CUnitObject& Prototype);
	virtual ~CCortexReaver() = default;

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
	void attack_explosive(_float fTimeDelta);
	void attack_melee(_float fTimeDelta);
	void attack_lightning(_float fTimeDelta);
	void attack_lightning_short(_float fTimeDelta);

	void CinematicDrop(_float fTimeDelta);

	// 각도가 좁혀지거나 거리가 좁혀졌을때, 무슨 공격을 할지 결정하는 함수가 있으면 좋겠다.
	// 원작에서도 플레이어 쫒아오니 그냥 AStar 쓰자.
	// 평타는 무슨 파란 구체를 뾰뵤뵹 발사하는걸 하는데  그걸 쓰자.
	// 무슨 수류탄을 마구 뿌리는 패턴이 있다. 그것 쓰자.
	// 어짜피 바로 발견하는 이벤트 나오니까 turn_To_Player는 뺄까.
private:
	Anim_State m_eAnimState = Idle;
	CortexReaver_State m_eActionState = STATE_STAND;
	const _float m_fMeleeDistance = 6.f;
	_int	m_iExplosiveBlowed = 0;
	_float m_fDropTime = 0.f;
	_float2 m_vShootCoolTime = { 0.f, 1.f};
	_float2 m_vGrenadeCoolTime = { 0.f, 6.f };
	_float3 m_vLightningCoolTime = { 0.f, 15.f, 8.f };
	_float2 m_vLightningShortCoolTime = { 0.f, 3.f, };
	_bool m_GrenadeFired = false;
public:
	static CCortexReaver* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
	
};

END