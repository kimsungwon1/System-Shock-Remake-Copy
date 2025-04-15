#pragma once

#include "Client_Defines.h"
#include "ContainObject.h"

BEGIN(Engine)
class CShader;
class CModel;
class CNavigation;
class CCollider;
class CPhysics;
END

BEGIN(Client)

class CUnitObject : public CContainObject
{
	enum SKINTYPE {
		TYPE_FLESH,
		TYPE_MACHINE,
		TYPE_END,
	};
protected:
	CUnitObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUnitObject(const CUnitObject& Prototype);
	virtual ~CUnitObject() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	_bool IsPicking(_float3* pOut);
	static _bool TileIntersectsWithTile(_fvector pos, _fvector dir, _float fDistance);
	CCollider* Get_Collider() { return m_pColliderCom; }
	CCollider* Get_HeadCollider() { return m_pHeadColliderCom; }

	void SetPositionNavi();
protected:
	_bool TileBetweenMeAndPlayer();
	// 여기 상태-함수는 무조건 _bool을 리턴한다. _bool은 false면 끝났다는 것이다.
protected:
	//애니메이션 끝날때, 항상 Idle 애니메이션을 거는 함수. 자식이 반드시 재정의 해야 한다.
	virtual _bool setIdle(_float fTimeDelta) = 0;
	//죽었으면 Idle보다 먼저 호출해 죽어 있게함.
	virtual _bool setDead(_float fTimeDelta) = 0;

	_bool m_bMoving_By_AStar = {};
	_float3 m_vDestPos = {};
	virtual _bool move_By_AStar(_float fTimeDelta);

	_bool m_bFinding_Player = {};
	_float m_fSight = 10.f; // 시야 거리. 플레이어를 찾을수 있냐 여부.
	virtual _bool find_Player(_float fTimeDelta);

	_bool m_bTurning_To_Player = {};
	virtual _bool turn_To_Player(_float fTimeDelta);

	_bool m_bAttacking_Normal = {};
	virtual _bool attack_normal(_float fTimeDelta) { return false; }

	virtual void find_path();
	_bool ThereIsAllyFront(_fvector vDir, _float fDistance);

	_bool IsDestinationDirectly();

	_float m_fDeadTime = 1.f;
	_int m_iHP = {};
public:
	void Set_Damage(_int iDamage) { 
		m_iHP -= iDamage; 
	}
	UnitType GetUnitType() const { return m_eUnitType; }
protected:
	CGameObject* m_pTargetObject = { nullptr };

	_bool m_bAnimationFinished = {};
	_float m_fDealDistance = {};
	UnitType m_eUnitType = {};
protected:
	CShader* m_pShaderCom = { nullptr };
	CModel* m_pModelCom = { nullptr };
	CNavigation* m_pNavigationComs[L_END] = { nullptr, };
	CCollider* m_pColliderCom = { nullptr };
	CCollider* m_pHeadColliderCom = { nullptr };
	CPhysics* m_pPhysicsCom = { nullptr };
public:
	virtual CGameObject* Clone(void* pArg) = 0;
	virtual void Free() override;

};
END
