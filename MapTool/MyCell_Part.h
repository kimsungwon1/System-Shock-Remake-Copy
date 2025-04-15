#pragma once
#include "Base.h"

BEGIN(Engine)
class CVIBuffer;
END

class CMyCell_Part abstract : public CBase
{
protected:
	CMyCell_Part(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CMyCell_Part() = default;

public:
	virtual HRESULT Initialize(const _float3* pPoints, _int iIndex) = 0;

public:
	virtual HRESULT Render() { return S_OK; }

	virtual _bool IsPicking(const _float4x4* naviWorldMat, _float3* pOut) { return false; }
protected:
	ID3D11Device* m_pDevice = nullptr;
	ID3D11DeviceContext* m_pContext = nullptr;
	
	CVIBuffer* m_pVIBuffer = nullptr;

	_float4x4			m_NavigationWorldMatrix = {};
	_int m_iIndex = {};
public:
	void Free() override;
};

