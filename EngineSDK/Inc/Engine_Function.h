#pragma once

namespace Engine
{
	template<typename T>
	void Safe_Delete(T& pPointer)
	{
		if (nullptr != pPointer)
		{
			delete pPointer;
			pPointer = nullptr;
		}
	}

	template<typename T>
	void Safe_Delete_Array(T& pArray)
	{
		if (nullptr != pArray)
		{
			delete[] pArray;
			pArray = nullptr;
		}
	}

	template<typename T>
	unsigned int Safe_Release(T& pInstance)
	{
		unsigned int iRefCnt = 0;

		if (nullptr != pInstance)
		{
			iRefCnt = pInstance->Release();

			if(0 == iRefCnt)
				pInstance = nullptr;
		}

		return iRefCnt;
	}

	template<typename T>
	unsigned int Safe_AddRef(T& pInstance)
	{
		unsigned int iRefCnt = 0;

		if (nullptr != pInstance)	
			iRefCnt = pInstance->AddRef();			

		return iRefCnt;
	}
	
	typedef pair<float, float> pff;

	template<typename T>
	void Myswap(T& ref1, T& ref2) {
		T tmp = ref2;
		ref2 = ref1;
		ref1 = tmp;
	}

	static int ccw(pff p1, pff p2, pff p3) {
		int s = (int)(p1.first * p2.second + p2.first * p3.second + p3.first * p1.second);
		s -= (int)(p1.second * p2.first + p2.second * p3.first + p3.second * p1.first);

		if (s > 0) return 1;
		else if (s == 0) return 0;
		else return -1;
	}

	static bool isIntersect(pair<pff, pff> l1, pair<pff, pff> l2) {

		pff p1 = l1.first;
		pff p2 = l1.second;
		pff p3 = l2.first;
		pff p4 = l2.second;

		int p1p2 = ccw(p1, p2, p3) * ccw(p1, p2, p4); // l1 기준
		int p3p4 = ccw(p3, p4, p1) * ccw(p3, p4, p2); // l2 기준

		// 두 직선이 일직선 상에 존재
		if (p1p2 == 0 && p3p4 == 0) {
			// 비교를 일반화하기 위한 점 위치 변경
			if (p1 > p2) Myswap(p2, p1);
			if (p3 > p4) Myswap(p3, p4);

			return p3 <= p2 && p1 <= p4; // 두 선분이 포개어져 있는지 확인
		}

		return p1p2 <= 0 && p3p4 <= 0;
	}
}



