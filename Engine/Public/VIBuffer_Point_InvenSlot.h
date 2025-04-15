#pragma once
#include "VIBuffer_Instancing.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Point_InvenSlot final : public CVIBuffer_Instancing
{
public:
	struct INVENSLOTINSTANCE_DESC {
		_float2 arrSlotPoses[5][12] = {};
		_float2 vScale = {};
	};
private:
	CVIBuffer_Point_InvenSlot(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CVIBuffer_Point_InvenSlot(const CVIBuffer_Point_InvenSlot& Prototype);
	virtual ~CVIBuffer_Point_InvenSlot() = default;

public:
	virtual HRESULT Initialize_Prototype(const CVIBuffer_Instancing::INSTANCE_DESC& Desc) override;
	virtual HRESULT Initialize(void* pArg) override;

public:
	// x - 0 ~ 11, y - 0 ~ 4
	_float2 Get_Pos(_int x, _int y) {
		return m_arrSlotPoses[y][x];
	}

private:
	_float2 m_arrSlotPoses[5][12] = {};
	_float2 m_vScale = {};

public:
	static CVIBuffer_Point_InvenSlot* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const CVIBuffer_Instancing::INSTANCE_DESC& Desc);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;

};

END