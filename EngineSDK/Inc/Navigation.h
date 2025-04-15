#pragma once
#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CNavigation : public CComponent
{
public:
	struct NAVIGATION_DESC
	{
		_int	iCurrentIndex = { -1 };
	};
private:
	CNavigation(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CNavigation(const CNavigation& Prototype);
	virtual ~CNavigation() = default;

public:
	virtual HRESULT Initialize_Prototype(const _wstring& strNavigationDataFile);
	virtual HRESULT Initialize(void* pArg) override;

public:
	void Update(_fmatrix TerrainWorldMatrix);
	_bool isMove(_fvector vPosition, _fvector vOriPosition, _float* fHeight, class CCollider* pCollider, _float3* vLineOut);

	_int SetCurrentIndex(_vector vertex, class CCollider* pCollider);
	_bool IsFloating(_fvector vPosition, _float* fHeight = nullptr);

	_float Get_Height(_vector vPosition) const;
	_vector Get_CurrentCell_Normal() const;

#ifdef _DEBUG
public:
	HRESULT Render();
#endif

private:
	vector<class CCell*>	m_Cells;
	_int					m_iCurrentCellIndex = { -1 };
	static	_float4x4		m_WorldMatrix;

public:
	static const _float		s_fFriction;

	_int Get_CurrentCellIndex() const { return m_iCurrentCellIndex; }

#pragma region AStar
public:
	void AStar();
	void Set_FinalDest(_fvector pos);
	pair<_float3, _float3> Get_NextLine() const;
	_vector Get_NextCellPos() const;	//reservedPath가 비었을때 호출하면 안됨
	_bool IsReservedEmpty() const { return m_reservedPath.empty(); }

	void Initialize_AStar() { m_reservedPath.clear(); }

	list<class CCollider*> Get_ColliderBelonged() const;
private:
	list<_int>	m_reservedPath = {};
	_int	m_iDestCell = { -1 };
#pragma endregion

#ifdef _DEBUG
private:
	class CShader* m_pShader = { nullptr };
#endif

private:
	HRESULT SetUp_Neighbors();

public:
	static CNavigation* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _wstring& strNavigationDataFile);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};
END
