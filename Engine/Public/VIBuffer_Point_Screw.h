#pragma once
#include "VIBuffer_Instancing.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Point_Screw : public CVIBuffer_Instancing
{
public:
	struct SCREWINSTANCE_DESC {
		const class CGameObject* pOwner = nullptr;
		_float fLookOffset;

	};
private:
	CVIBuffer_Point_Screw(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CVIBuffer_Point_Screw(const CVIBuffer_Point_Screw& Prototype);
	virtual ~CVIBuffer_Point_Screw() = default;

public:
	virtual HRESULT Initialize_Prototype(const CVIBuffer_Instancing::INSTANCE_DESC& Desc) override;
	virtual HRESULT Initialize(void* pArg) override;

public:
	virtual void Spread(_float fTimeDelta) override;
	virtual void Drop(_float fTimeDelta) override;
	void Screw(_float fTimeDelta);
private:
	_float m_fLookOffset = {};
	_float m_fTimePass = { 0.f };
	const class CGameObject* m_pOwner = nullptr;
public:
	static CVIBuffer_Point_Screw* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const CVIBuffer_Instancing::INSTANCE_DESC& Desc);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END