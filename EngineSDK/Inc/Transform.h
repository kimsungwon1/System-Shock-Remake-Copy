#pragma once

#include "Component.h"

BEGIN(Engine)

class CNavigation;
class CCollider;

class ENGINE_DLL CTransform final : public CComponent
{
public:
	typedef struct
	{
		_float		fSpeedPerSec{}, fRotationPerSec{};
	}TRANSFORM_DESC;

	enum STATE { STATE_RIGHT, STATE_UP, STATE_LOOK, STATE_POSITION, STATE_END };
	enum DIR { DIR_FRONT = 0, DIR_BACK, DIR_LEFT, DIR_RIGHT, DIR_STOP, DIR_END };
private:
	CTransform(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CTransform() = default;

public:
	void Set_State(STATE eState, _fvector vState) {
		XMStoreFloat3((_float3*)&m_WorldMatrix.m[eState][0], vState);
	}

	_vector Get_State(STATE eState) const {
		return XMLoadFloat4x4(&m_WorldMatrix).r[eState];
	}

	_float3 Get_Scaled() const;

	_matrix Get_WorldMatrix_Inverse() const {
		return XMMatrixInverse(nullptr, XMLoadFloat4x4(&m_WorldMatrix));
	}

	const _float4x4* Get_WorldMatrix_Ptr() const {
		return &m_WorldMatrix;
	}

public:	
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	
public:
	void Set_Scaled(_float fX, _float fY, _float fZ);
	void LookAt(_fvector vAt);
	void LookAt_ground(_fvector vAt);
	
	void Go_Straight(_float fTimeDelta, CNavigation* pNavigation = nullptr, CCollider* pMyCollider = nullptr);
	void Go_Backward(_float fTimeDelta, CNavigation* pNavigation = nullptr, CCollider* pMyCollider = nullptr);
	void Go_Left(_float fTimeDelta, CNavigation* pNavigation = nullptr, CCollider* pMyCollider = nullptr);
	void Go_Right(_float fTimeDelta, CNavigation* pNavigation = nullptr,  CCollider* pMyCollider = nullptr);
	void Go_Up(_float fTimeDelta, CNavigation* pNavigation = nullptr, CCollider* pMyCollider = nullptr);
	void Go_Down(_float fTimeDelta, CNavigation* pNavigation = nullptr, CCollider* pMyCollider = nullptr);

	// iDir 0 : ╬у, 1 : ╣з, 2 : аб, 3 : ©Л
	_vector Walk(_int iDir, _float fTimeDelta, CNavigation* pNavigation = nullptr, _float3* vMoved = nullptr, class CCollider* pMyCollider = nullptr);

	void Turn(_fvector vAxis, _float fTimeDelta);	
	void Turn(_bool isRotationX, _bool isRotationY, _bool isRotationZ, _float fTimeDelta);
	void Rotation(_fvector vAxis, _float fRadian);
	void Rotation(_float fX, _float fY, _float fZ, _float fRadian);

	void SetLookToRightUp(_fvector vLook);

	void SetLookToRightUp_ground(_fvector vLook);


	void SetWorldMat(const _float4x4& mat) { m_WorldMatrix = mat; }
	_float4x4 GetWorldMat() const { return m_WorldMatrix; }
	_matrix GetWorldMat_XMMat() const { return XMLoadFloat4x4(&m_WorldMatrix); }
public:
	HRESULT Bind_ShaderResource(class CShader* pShader, const _char* pConstantName);

	_vector Bind_Physics(_float fTimeDelta, class CPhysics* pPhysics, class CNavigation* pNavigation, class CCollider* pMyCollider = nullptr);
public:
	_float GetSpeedPerSec() const { return m_fSpeedPerSec; }
	_float GetRotationPerSec() const { return m_fRotationPerSec; }

private:
	_float			m_fSpeedPerSec{};
	_float			m_fRotationPerSec{};
	_float4x4		m_WorldMatrix = {};

public:
	static CTransform* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg);
	virtual CComponent* Clone(void* pArg) { return nullptr; }
	virtual void Free() override;
};

END