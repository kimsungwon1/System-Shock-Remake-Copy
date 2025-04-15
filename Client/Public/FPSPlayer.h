#pragma once
#include "Client_Defines.h"
#include "ContainObject.h"

BEGIN(Engine)
class CNavigation;
class CCollider;
class CPhysics;
END


BEGIN(Client)

class CFPSPlayer final : public CContainObject
{
public:
	enum PartID { PART_BODY, PART_WEAPON, PART_OBJECT, PART_END };
	enum PLAYER_STATE {

		STATE_RESET = 0x00000000,
		STATE_STAND = 0x00000001,
		STATE_WALK = 0x00000002,
		STATE_RUN = 0x00000004,
		STATE_ATTACK = 0x00000008,
		STATE_JUMP = 0x00000010,
		STATE_RELOAD = 0x00000020,

		STATE_END = 0xffffffff,
	};

	enum ANIM_STATE {
		ASSAULTRIFLE_DRAW = 56,
		ASSAULTRIFLE_HOLSTER = 58,
		ASSAULTRIFLE_IDLE = 60,
		ASSAULTRIFLE_IDLE_AIM = 61,
		ASSAULTRIFLE_IDLE_MOVING = 67,
		ASSAULTRIFLE_IDLE_SPRINT = 69,
		ASSAULTRIFLE_RELOAD = 72,
		ASSAULTRIFLE_SHOOT = 81,

		Punch_Left = 332,
		Punch_Right = 333,

		P96_Draw = 510,
		P96_Idle = 512,
		P96_Moving = 515,
		P96_Reload = 517,
		P96_Shoot = 518,

		Throw = 803,

		DO_NOTHING = 724,

		ANIM_END
	};
private:
	CFPSPlayer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CFPSPlayer(const CFPSPlayer& Prototype);
	virtual ~CFPSPlayer() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

	static _float Get_Height() { return s_fHeight; }

	CCollider* Get_Collider() { return m_pColliderCom; }

	_bool IsMoving() const { return m_bMoving; }

	void ReceivePhysicsPower(_fvector vPower);

	void SetPositionNavi(_fvector vPosition);
private:
	void control_With_AssaultRifle(class CWeapon* pWeapon, _float fTimeDelta);
	void control_With_Pistol(class CWeapon* pWeapon, _float fTimeDelta);
	void control_With_RocketLauncher(class CWeapon* pWeapon, _float fTimeDelta);
	void control_With_Fist(_float fTimeDelta);

	HRESULT switch_Weapon(_int iIndex);
	void set_Reload_Anim();

	void reloadWeapon();

	_bool isReloading();

	void beRecoiled(const _float2& vRecoil);
public:
	_bool IsThisDoorOpenable(_int iDoorData);
private:
	_uint m_iState = {};
	_float m_fAttackedTime = -1.f;
	_float			m_fJumpPerSec = { 5.0f };

	_float			m_fPitchY = 0.f;

	_float2			m_vRecoiled = { 0.f, 0.f };

	_bool			m_bGrenadeThrowing = false;
	_float2			m_vGrenadeCoolTime = { 0.f, 1.f };

	class CFPSCamera* m_pFPSCamera = nullptr;
	CNavigation* m_pNavigationComs[GAMEPLAY_LEVELID::L_END] = { nullptr, };
	//CNavigation* m_pCurrentNavigationCom = nullptr;

	CCollider* m_pColliderCom = nullptr;
	CPhysics* m_pPhysicsCom = nullptr;

	class CFPS_Arm* m_pArm = nullptr;

	class CWeapon* m_Weapons[10] = { nullptr, };

	class CAnimationText* m_pAnimText = nullptr;

	class CInventory* m_pInventory = nullptr;

	_bool m_bTurnedInven = true;

	vector<class CCard*> m_vecCards = {};

	static _float s_fHeight;

	_bool m_bMoving = false;
	_bool m_bLightOnOff = false;
	_bool m_bReloading = false;

	LIGHT_DESC m_LanternDesc = {};

	_int m_iCurrentWeapon = -1;
	_int m_iHP = 24;
	_int m_iCurrentLoad = 0;
public:
	void ReceiveDamage(_int iDamage);
	_int Get_HP() const { return m_iHP; }
	_int Get_CurrentWeaponLoad() const { return m_iCurrentLoad; }

private:
	HRESULT Ready_Components();
	HRESULT Ready_PartObjects();
public:
	class CFPS_Arm* GetArm();
public:
	static CFPSPlayer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;

};

END

