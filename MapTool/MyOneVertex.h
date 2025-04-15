#pragma once
#include "MyCell_Part.h"
class CMyOneVertex : public CMyCell_Part
{
private:
	CMyOneVertex(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CMyOneVertex() = default;

public:
	HRESULT Initialize(const _float3* pPoints, _int iIndex) override;

	virtual HRESULT Render() override;

private:
	_float3 m_vPosition;
public:
	static CMyOneVertex* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _float3* pPoints, _int iIndex);
	virtual void Free() override;
};

