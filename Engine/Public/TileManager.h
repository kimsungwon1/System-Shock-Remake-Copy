#pragma once
#include "Base.h"
class CTileManager final : public CBase
{
private:
	CTileManager() = default;
	~CTileManager() = default;

public:
	HRESULT Add_Tile(class CTileObject* pTileObject) {
		m_vecTiles.push_back(pTileObject);
	}
	// ���߿� ���� ���� �˻��ϵ��� �ؾ� �Ѵ�.
	_bool LineIntersects(_fvector vWorldRay_Pos, _fvector vWorldRay_Dir, _float* pfDistance);

private:
	vector<class CTileObject*> m_vecTiles = {};

public:
	static CTileManager* Create();
	void Free();
};

