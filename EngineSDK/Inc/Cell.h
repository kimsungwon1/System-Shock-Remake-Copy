#pragma once

#include "Base.h"

BEGIN(Engine)
class ENGINE_DLL CCell : public CBase
{
public:
	enum POINT { POINT_A, POINT_B, POINT_C, POINT_END };
	enum LINE { LINE_AB, LINE_BC, LINE_CA, LINE_END };
private:
	CCell(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CCell() = default;

public:
	_vector Get_Point(POINT ePoint) const {
		return XMLoadFloat3(&m_vPoints[ePoint]);
	}

	_int Get_Neighbor(LINE eLine) const {
		return m_iNeighborIndices[eLine];
	}

	void Set_Neighbor(LINE eLine, CCell* pNeighbor) {
		m_iNeighborIndices[eLine] = pNeighbor->m_iIndex;
	}

	_vector Get_Center() const;

public:
	HRESULT Initialize(const _float3* pPoints, _int iIndex);
	_bool Compare_Points(_fvector vSour, _fvector vDest);
	_bool isIn(_fvector vPosition, int* pNeighborIndex, _float3& vLine, _int iCurrentCellIndex);
	_bool isIn(_fvector vPosition);

	_float GetHeight(_float x, _float z);
	_vector Get_Normal() { return XMPlaneFromPoints(XMLoadFloat3(&m_vPoints[0]), XMLoadFloat3(&m_vPoints[1]), XMLoadFloat3(&m_vPoints[2])); }

	void Push_Collider(class CCollider* pCollider) { m_CollidersBelonged.push_back(pCollider); }
	void Erase_Collider(class CCollider* pCollider);
	const list<class CCollider*>& Get_Colliders() const { return m_CollidersBelonged; }
#ifdef _DEBUG
public:
	HRESULT Render();
#endif

private:
	ID3D11Device* m_pDevice = nullptr;
	ID3D11DeviceContext* m_pContext = nullptr;
	_int				m_iIndex = {};
	_float3				m_vPoints[POINT_END] = {};
	_int				m_iNeighborIndices[LINE_END] = { -1, -1, -1 };

	list<class CCollider*> m_CollidersBelonged = {};
	//선분 하나라도 충돌하며 위에 있는 셀들.
	vector<CCell*> m_vecAboveCells = {};
public:
	void Set_AboveCells(CCell* pCell);
#ifdef _DEBUG
private:
	class CVIBuffer_Cell* m_pVIBuffer = nullptr;
#endif

public:
	static CCell* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _float3* pPoints, _int iIndex);
	virtual void Free() override;
};
END
