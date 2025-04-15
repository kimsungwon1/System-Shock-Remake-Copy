#pragma once
#include <wrl.h>

#include "MyCell_Part.h"

class CMyCell_Line : public CMyCell_Part
{
private:
	CMyCell_Line(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CMyCell_Line() = default;

public:
	_vector Get_Point(_int iPoint) const {
		return XMLoadFloat3(&m_vPoints[iPoint]);
	}

public:
	HRESULT Initialize(const _float3* pPoints, _int iIndex) override;
private:
	HRESULT Initialize_Line(_matrix naviMat, _vector vColor);

public:
	HRESULT Render() override;

private:
	DirectX::CommonStates* m_pStates;
	DirectX::BasicEffect* m_pEffect;
	DirectX::PrimitiveBatch<DirectX::VertexPositionColor> * m_pBatch;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> m_inputLayout;

	_float3				m_vPoints[2] = {};
	_float4				m_vColor = {};
	_float4x4			m_NavigationWorldMatrix = {};
public:
	static CMyCell_Line* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, 
		const _float3* pPoints, _int iIndex, _matrix naviMat, _vector vColor);
	virtual void Free() override;
};

