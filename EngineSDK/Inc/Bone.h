#pragma once
#include "Base.h"

BEGIN(Engine)

class CBone :
    public CBase
{
private:
    CBone() = default;
    ~CBone() = default;

public:
    HRESULT Initialize(const aiNode* pNode, _int iParentIndex);

public:
    const char* Get_Name() const { return m_szName; }

    _matrix Get_CombinedTransformationMatrix() const {
        return XMLoadFloat4x4(&m_CombinedTransformMatrix);
    }

    const _float4x4* Get_CombinedTransformMatrix_Ptr() const {
        return &m_CombinedTransformMatrix;
    }

    void Set_TransformationMatrix(_fmatrix TransformationMatrix) {
        XMStoreFloat4x4(&m_TransformationMatrix, TransformationMatrix);
    }

    void Update_CombinedTransformationMatrix(vector<class CBone*> Bones, _matrix preTransformMatrix);

    HRESULT SaveBone(ofstream& saveStream);
    HRESULT LoadBone(ifstream& loadStream);
private:
    _char m_szName[MAX_PATH] = {};

    _int m_iParentIndex = { -1 };

    /*부모기준으로 표현된 이 뼈만의 상태행렬*/
    _float4x4 m_TransformationMatrix = {};

    _float4x4 m_CombinedTransformMatrix = {};

public:
    static CBone* Create(const aiNode* pNode, _int iParentIndex);
    static CBone* Create(ifstream& loadStream);
    CBone* Clone();
    void Free() override;
};

END
