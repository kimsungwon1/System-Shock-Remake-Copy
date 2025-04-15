#pragma once
#include "UnitObject.h"

class CCyborgAssassin final : public CUnitObject
{
public:
	enum Anim_State {
		Melee_Attack,
		Range_Attack,
		Throw_Attack,
		Death,
		Idle,
		Run_Back,
		Run_Front,
		Run_Left,
		Run_Right,
		Sprint_Front,
		Turn_Left_180,
		Turn_Left_90,
		Turn_Right_0,
		Turn_Right_180,
		Turn_Right_90,
		Walk_Back,
		Walk_Front,
		Walk_Left,
		Walk_Right,
		ANIM_END
	};
	enum ASSASSIN_STATE {
		STATE_RESET =	0x00000000,
		STATE_STAND =	0x00000001,
		STATE_WALK =	0x00000002,
		STATE_TURN =	0x00000004,
		STATE_ATTACK =	0x00000008,
		STATE_END =		0xffffffff
	};
private:
	CCyborgAssassin(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCyborgAssassin(const CUnitObject& Prototype);
	virtual ~CCyborgAssassin() = default;

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
	_float4 m_vShootDuration = { 0.f, 0.4f, 1.f, 1.f };
public:
	static CCyborgAssassin* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

