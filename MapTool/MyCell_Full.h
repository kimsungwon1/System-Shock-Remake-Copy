#pragma once
#include "MyCell_Part.h"
class CMyCell_Full :
    public CMyCell_Part
{
public:
	enum POINT { POINT_A, POINT_B, POINT_C, POINT_END };
	enum LINE { LINE_AB, LINE_BC, LINE_CA, LINE_END };
private:
	CMyCell_Full(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CMyCell_Full() = default;

public:
	_vector Get_Point(POINT ePoint) const {
		return XMLoadFloat3(&m_vPoints[ePoint]);
	}

public:
	HRESULT Initialize(const _float3* pPoints, _int iIndex) override;

	virtual _bool IsPicking(const _float4x4* naviWorldMat, _float3* pOut) override;

	HRESULT SaveCell(ofstream& ofs);
public:
	HRESULT Render() override;

private:
	_float3				m_vPoints[POINT_END] = {};
public:
	static CMyCell_Full* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _float3* pPoints, _int iIndex);
	virtual void Free() override;
};

