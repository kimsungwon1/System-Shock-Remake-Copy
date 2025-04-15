#pragma once
#include "VIBuffer_Instancing.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Point_Screw_Short : public CVIBuffer_Instancing
{
public:
	struct SCREWINSTANCE_DESC {
		const class CGameObject* pOwner = nullptr;
		_float fLookOffset;
		_float fSpeed;
	};
private:
	CVIBuffer_Point_Screw_Short(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CVIBuffer_Point_Screw_Short(const CVIBuffer_Point_Screw_Short& Prototype);
	virtual ~CVIBuffer_Point_Screw_Short() = default;

public:
	virtual HRESULT Initialize_Prototype(const CVIBuffer_Instancing::INSTANCE_DESC& Desc) override;
	virtual HRESULT Initialize(void* pArg) override;

public:
	virtual void Spread(_float fTimeDelta) override;
	virtual void Drop(_float fTimeDelta) override;
	void Screw(_float fTimeDelta);

	_vector Get_Position() { return XMLoadFloat3(&m_vOriginPos) + m_fLookOffset * XMLoadFloat3(&m_vOriginLook); }
	_vector Get_LastPosition() { return Get_Position() += m_iNumInstance * m_fPerLookDistance * XMLoadFloat3(&m_vOriginLook); }
	_vector Get_Look() { return XMLoadFloat3(&m_vOriginLook); }
private:
	_float m_fTimePass = 0.f;

	_float m_fLookOffset = {};
	_float m_fAllSpeed = { 10.f };
	_float3 m_vOriginLook = {};
	_float3 m_vOriginPointLook = {};
	_float3 m_vOriginPos = {};
	const _float m_fPerLookDistance = 0.01f;
	const class CGameObject* m_pOwner = nullptr;
public:
	static CVIBuffer_Point_Screw_Short* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const CVIBuffer_Instancing::INSTANCE_DESC& Desc);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END