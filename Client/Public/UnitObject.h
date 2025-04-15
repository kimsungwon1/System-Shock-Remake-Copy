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
	// ���� ����-�Լ��� ������ _bool�� �����Ѵ�. _bool�� false�� �����ٴ� ���̴�.
protected:
	//�ִϸ��̼� ������, �׻� Idle �ִϸ��̼��� �Ŵ� �Լ�. �ڽ��� �ݵ�� ������ �ؾ� �Ѵ�.
	virtual _bool setIdle(_float fTimeDelta) = 0;
	//�׾����� Idle���� ���� ȣ���� �׾� �ְ���.
	virtual _bool setDead(_float fTimeDelta) = 0;

	_bool m_bMoving_By_AStar = {};
	_float3 m_vDestPos = {};
	virtual _bool move_By_AStar(_float fTimeDelta);

	_bool m_bFinding_Player = {};
	_float m_fSight = 10.f; // �þ� �Ÿ�. �÷��̾ ã���� �ֳ� ����.
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
