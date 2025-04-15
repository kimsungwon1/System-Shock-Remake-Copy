#pragma once
#include "GameObject.h"
class CMyNavigation : public CGameObject
{
private:
	CMyNavigation(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMyNavigation(const CMyNavigation& Prototype);
	virtual ~CMyNavigation() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg) override;

	void Late_Update(_float fTimeDelta) override;
	HRESULT Render() override;

public:
	//void Update(_fmatrix TerrainWorldMatrix);
	//_bool isMove(_fvector vPosition);

	HRESULT LoadFile(const _wstring& strNavigationDataFile);

public:
	HRESULT AddCellFull(const _float3* pVec);
	HRESULT AddCellLine(const _float3* pVec);

	HRESULT DeleteCell(const class CMyCell_Full* pDel);

	HRESULT ClearLines();
	HRESULT ClearAll();

	static HRESULT SortCell(_float3* pVec);

	class CMyCell_Full* Cell_Picking(_fvector vCamPos, _float3* pOut);

	_float3 Get_AlmostSamePoint(const _float3& pIn) const;

	HRESULT SaveNavi(ofstream& ofs);
	HRESULT LoadNavi(ifstream& ifs);
private:
	list<class CMyCell_Full*>	m_Cells = {};
	list<class CMyCell_Full*>	m_Lines = {};
private:
	class CShader* m_pShader = { nullptr };

private:
	HRESULT SetUp_Neighbors();

public:
	static CMyNavigation* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;

};

