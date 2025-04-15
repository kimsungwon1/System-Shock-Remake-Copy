#pragma once
#include "UnitObject.h"

BEGIN(Client)
class CDefaultMutant final : public CUnitObject
{
public:
	enum Anim_State {
		Hit,
		Eat_from_idle,
		Attack_a,	//right arm
		Attack_b,	//left	arm
		Charge,
		Charge_attack,
		Death_h,
		Death_p,
		Idle,
		Idle_fidget,
		Idle_eat,
		Eat_to_idle,
		Run_back,
		Run_front,
		Run_left,
		Run_right,
		Turn_l_180,
		Turn_l_90,
		Turn_r_0,
		Turn_r_180,
		Turn_r_90,
		Walk_b,
		Walk_f,
		Walk_l,
		Walk_r,
		ANIM_END
	};
	enum Mutant_State {
		STATE_RESET,
		STATE_STAND,
		STATE_WALK,
		STATE_TURN,
		STATE_ATTACK,
		STATE_ATTACK_CHARGE,
		STATE_END
	};

private:
	CDefaultMutant(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CDefaultMutant(const CUnitObject& Prototype);
	virtual ~CDefaultMutant() = default;

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

	virtual _bool turn_To_Player(_float fTimeDelta) override;

	_bool m_bLeftHitting = {};
	_bool m_bRightHitting = {};
	_bool hit_Player(_float fTimeDelta);

	virtual _bool move_By_AStar(_float fTimeDelta) override;
	virtual _bool attack_normal(_float fTimeDelta) override;
	_bool m_bAttacking_charge = {};
	_bool attack_charge(_float fTimeDelta);
		
	_bool attack_hit(_float fTimeDelta);
private:
	void update_TwoHands();
	void PlayAttackSound();
private:
	Anim_State m_eAnimState = Idle;
	Mutant_State m_eActionState = STATE_STAND;

	CCollider* m_pLeftMeleeColliderCom = nullptr;
	CCollider* m_pRightMeleeColliderCom = nullptr;

	_float		m_fChargeDistance = 7.f;
public:
	static CDefaultMutant* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;


};
END
