#include "Octree.h"
#include "GameObject.h"

#include <numeric>

#undef max;
#undef min;

COctree::COctree()
{
}

_vector COctree::Get_OCT_Dir(OCT_TYPE eDir)
{
	_vector ret = {};
	switch (eDir) {
	case UP_NW:
		ret = { -1.f, 1.f, 1.f };
		break;
	case UP_NE:
		ret = { 1.f, 1.f, 1.f };
		break;
	case UP_SE:
		ret = { 1.f, 1.f, -1.f };
		break;
	case UP_SW:
		ret = { -1.f, 1.f, -1.f };
		break;
	case DOWN_NW:
		ret = { -1.f, -1.f, 1.f };
		break;
	case DOWN_NE:
		ret = { 1.f, -1.f, 1.f };
		break;
	case DOWN_SE:
		ret = { 1.f,-1.f,-1.f };
		break;
	case DOWN_SW:
		ret = { -1.f, -1.f, -1.f };
		break;
	}

	return ret;
}

HRESULT COctree::Initialize(const list<CGameObject*>& listVariable, _float fMinRectLength)
{
	m_fMinRectLength = fMinRectLength;

	_float minX = numeric_limits<_float>::max(); _float maxX = numeric_limits<_float>::min();
	_float minY = numeric_limits<_float>::max(); _float maxY = numeric_limits<_float>::min();
	_float minZ = numeric_limits<_float>::max(); _float maxZ = numeric_limits<_float>::min();

	_float fXDistance = {}; _float fYDistance = {}; _float fZDistance = {};
	for (auto& pObj : listVariable) {
		_vector vPos = pObj->GetTransformCom()->Get_State(CTransform::STATE_POSITION);
		if (XMVectorGetX(vPos) < minX) {
			minX = XMVectorGetX(vPos);
		}
		if (XMVectorGetY(vPos) < minY) {
			minY = XMVectorGetY(vPos);
		}
		if (XMVectorGetZ(vPos) < minZ) {
			minZ = XMVectorGetZ(vPos);
		}
		if (XMVectorGetX(vPos) > maxX) {
			maxX = XMVectorGetX(vPos);
		}
		if (XMVectorGetY(vPos) > maxY) {
			maxY = XMVectorGetY(vPos);
		}
		if (XMVectorGetZ(vPos) > maxZ) {
			maxZ = XMVectorGetZ(vPos);
		}
	}

	_vector vCenter = XMVectorSet((maxX + minX) * 0.5f, (maxY + minY) * 0.5f, (maxZ + minZ) * 0.5f, 1.f);
	XMStoreFloat3(&m_vCenter, vCenter);

	fXDistance = maxX - minX; fYDistance = maxY - minY; fZDistance = maxZ - minZ;

	_float fLength = fMinRectLength;

	while (fLength < fXDistance || fLength < fYDistance || fLength < fZDistance) {
		fLength *= 2.f;
	}

	m_fRectLength = fLength;
	
	//나누기
	for (_int i = 0; i < 8; i++) {
		_vector vDir = Get_OCT_Dir((OCT_TYPE)i) * m_fRectLength * 0.25f;
		_vector vChildCenter = vDir + vCenter;
		list<CGameObject*> list;
		_float childMinX = XMVectorGetX(vChildCenter) - m_fRectLength * 0.25f; _float childMaxX = XMVectorGetX(vChildCenter) + m_fRectLength * 0.25f;
		_float childMinY = XMVectorGetY(vChildCenter) - m_fRectLength * 0.25f; _float childMaxY = XMVectorGetY(vChildCenter) + m_fRectLength * 0.25f;
		_float childMinZ = XMVectorGetZ(vChildCenter) - m_fRectLength * 0.25f; _float childMaxZ = XMVectorGetZ(vChildCenter) + m_fRectLength * 0.25f;
		for (auto& pObj : listVariable) {
			_vector vPos = pObj->GetTransformCom()->Get_State(CTransform::STATE_POSITION);
			if (childMinX < XMVectorGetX(vPos) && childMaxX >= XMVectorGetX(vPos) &&
				childMinY < XMVectorGetY(vPos) && childMaxY >= XMVectorGetY(vPos) &&
				childMinZ < XMVectorGetZ(vPos) && childMaxZ >= XMVectorGetZ(vPos))
			{
				list.push_back(pObj);
			}
		}

		COctree* pInstance = COctree::Create(list, m_fMinRectLength, m_fRectLength * 0.5f, vChildCenter);

		m_pChilds[i] = pInstance;
	}

	return S_OK;
}

HRESULT COctree::Initialize(const list<CGameObject*>& listVariable, _float fMinRectLength, _float fRectLength, _cvector vCenter)
{
	XMStoreFloat3(&m_vCenter, vCenter);

	m_fMinRectLength = fMinRectLength;

	m_fRectLength = fRectLength;

	if (listVariable.empty()) {
		return S_OK;
	}
	if (fMinRectLength >= m_fRectLength) {
		for (auto& pObj : listVariable) {
			m_listVar.push_back(pObj);
		}
		return S_OK;
	}
	//나누기
	for (_int i = 0; i < 8; i++) {
		_vector vDir = Get_OCT_Dir((OCT_TYPE)i) * m_fRectLength * 0.25f;
		_vector vChildCenter = vDir + vCenter;
		list<CGameObject*> list;
		_float minX = XMVectorGetX(vChildCenter) - m_fRectLength * 0.25f; _float maxX = XMVectorGetX(vChildCenter) + m_fRectLength * 0.25f;
		_float minY = XMVectorGetY(vChildCenter) - m_fRectLength * 0.25f; _float maxY = XMVectorGetY(vChildCenter) + m_fRectLength * 0.25f;
		_float minZ = XMVectorGetZ(vChildCenter) - m_fRectLength * 0.25f; _float maxZ = XMVectorGetZ(vChildCenter) + m_fRectLength * 0.25f;
		for (auto& pObj : listVariable) {
			_vector vPos = pObj->GetTransformCom()->Get_State(CTransform::STATE_POSITION);
			if (minX < XMVectorGetX(vPos) && maxX >= XMVectorGetX(vPos) &&
				minY < XMVectorGetY(vPos) && maxY >= XMVectorGetY(vPos) &&
				minZ < XMVectorGetZ(vPos) && maxZ >= XMVectorGetZ(vPos))
			{
				list.push_back(pObj);
			}
		}

		COctree* pInstance = COctree::Create(list, m_fMinRectLength, m_fRectLength * 0.5f, vChildCenter);

		m_pChilds[i] = pInstance;
	}

	return S_OK;
}

void COctree::Delete_Variable(CGameObject* pVar, _fvector vPosition)
{
	_bool bIntersects = false;

	_float minX = m_vCenter.x - m_fRectLength * 0.5f; _float maxX = m_vCenter.x + m_fRectLength * 0.5f;
	_float minY = m_vCenter.y - m_fRectLength * 0.5f; _float maxY = m_vCenter.y + m_fRectLength * 0.5f;
	_float minZ = m_vCenter.z - m_fRectLength * 0.5f; _float maxZ = m_vCenter.z + m_fRectLength * 0.5f;

	// 옥트리 안에 있는지 비교
	if (minX < XMVectorGetX(vPosition) && maxX >= XMVectorGetX(vPosition) &&
		minY < XMVectorGetY(vPosition) && maxY >= XMVectorGetY(vPosition) &&
		minZ < XMVectorGetZ(vPosition) && maxZ >= XMVectorGetZ(vPosition))
	{
		bIntersects = true;
	}

	if (!bIntersects) {
		return;
	}
	else {
		if (m_fRectLength <= m_fMinRectLength) {
			for (auto iter = m_listVar.begin(); iter != m_listVar.end();) {
				if (*iter == pVar) {
					iter = m_listVar.erase(iter);
				}
				else {
					iter++;
				}
			}
		}
		else {
			if (m_pChilds[0] == nullptr) {
				return;
			}

			for (_int i = 0; i < 8; i++) {
				m_pChilds[i]->Delete_Variable(pVar, vPosition);
			}
			return;
		}

	}
}

list<CGameObject*> COctree::GetList_ByRadius(_fvector vCenter, _float fRadius)
{
	_bool bIntersects = false;

	_float minX = m_vCenter.x - m_fRectLength * 0.5f; _float maxX = m_vCenter.x + m_fRectLength * 0.5f;
	_float minY = m_vCenter.y - m_fRectLength * 0.5f; _float maxY = m_vCenter.y + m_fRectLength * 0.5f;
	_float minZ = m_vCenter.z - m_fRectLength * 0.5f; _float maxZ = m_vCenter.z + m_fRectLength * 0.5f;

	// 옥트리 안에 있는지 비교
	if (minX < XMVectorGetX(vCenter) && maxX >= XMVectorGetX(vCenter) &&
		minY < XMVectorGetY(vCenter) && maxY >= XMVectorGetY(vCenter) &&
		minZ < XMVectorGetZ(vCenter) && maxZ >= XMVectorGetZ(vCenter))
	{
		bIntersects = true;
	}

	//옥트리 에서 가장 가까운 점과 구 비교, 평면 비교
	_float fCompX = {}; _float fCompY = {}; _float fCompZ = {};
	if (XMVectorGetX(vCenter) < minX) {
		fCompX = minX;
	}
	else if (XMVectorGetX(vCenter) > maxX) {
		fCompX = maxX;
	}
	else {
		fCompX = XMVectorGetX(vCenter);
	}

	if (XMVectorGetY(vCenter) < minY) {
		fCompY = minY;
	}
	else if (XMVectorGetY(vCenter) > maxY) {
		fCompY = maxY;
	}
	else {
		fCompY = XMVectorGetY(vCenter);
	}

	if (XMVectorGetZ(vCenter) < minZ) {
		fCompZ = minZ;
	}
	else if (XMVectorGetZ(vCenter) > maxZ) {
		fCompZ = maxZ;
	}
	else {
		fCompZ = XMVectorGetZ(vCenter);
	}

	_float fDistance = sqrtf(powf(fCompX - XMVectorGetX(vCenter), 2.f) + powf(fCompY - XMVectorGetY(vCenter), 2.f) + powf(fCompZ - XMVectorGetZ(vCenter), 2.f));

	if (fDistance < fRadius)
		bIntersects = true;

	// 충돌 없을 시 빈 리스트 리턴
	if (!bIntersects) {
		return list<CGameObject*>();
	}
	else {
		//옥트리의 길이가 현재 최소 이하이다 -> 재귀 끝
		if (m_fRectLength <= m_fMinRectLength) {
			return m_listVar;
		}
		else {
			list<CGameObject*> list = {};
			// 자식이 하나라도 없다-> 마지막 옥트리이다 -> 재귀 끝
			if (m_pChilds[0] == nullptr) {
				return list;
			}

			// 8개의 자식에게 재귀를 호출, 자식에게 있는 리스트를 더한다.
			for (_int i = 0; i < 8; i++) {
				auto newList = m_pChilds[i]->GetList_ByRadius(vCenter, fRadius);
				// 리스트 합
				list.splice(list.end(), newList);
			}
			return list;
		}

	}
}

void COctree::Update_Variable(const CGameObject* var)
{
}

void COctree::Free()
{
	__super::Free();

	for (_int i = 0; i < 8; i++) {
		Safe_Release(m_pChilds[i]);
	}
}
