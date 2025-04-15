#include "Bone.h"

HRESULT CBone::Initialize(const aiNode* pNode, _int iParentIndex)
{
    m_iParentIndex = iParentIndex;

    strcpy_s(m_szName, pNode->mName.data);

    memcpy(&m_TransformationMatrix, &pNode->mTransformation, sizeof(_float4x4));

    XMStoreFloat4x4(&m_TransformationMatrix, XMMatrixTranspose(XMLoadFloat4x4(& m_TransformationMatrix)));

    XMStoreFloat4x4(&m_CombinedTransformMatrix, XMMatrixIdentity());

    return S_OK;
}

void CBone::Update_CombinedTransformationMatrix(vector<class CBone*> Bones, _matrix preTransformMatrix)
{
    if (m_iParentIndex == -1)
        XMStoreFloat4x4(&m_CombinedTransformMatrix, XMLoadFloat4x4(&m_TransformationMatrix) * preTransformMatrix);
    else
        XMStoreFloat4x4(&m_CombinedTransformMatrix, XMLoadFloat4x4(&m_TransformationMatrix) * Bones[m_iParentIndex]->Get_CombinedTransformationMatrix());
}

HRESULT CBone::SaveBone(ofstream& saveStream)
{
    saveStream.write(reinterpret_cast<const char*>(&m_iParentIndex), sizeof(m_iParentIndex));
    
    saveStream.write(reinterpret_cast<const char*>(m_szName), sizeof(_char) * MAX_PATH);
    saveStream.write(reinterpret_cast<const char*>(&m_TransformationMatrix), sizeof(_float4x4));
    saveStream.write(reinterpret_cast<const char*>(&m_CombinedTransformMatrix), sizeof(_float4x4));

    return S_OK;
}

HRESULT CBone::LoadBone(ifstream& loadStream)
{
    loadStream.read(reinterpret_cast<char*>(&m_iParentIndex), sizeof(m_iParentIndex));

    loadStream.read(reinterpret_cast<char*>(m_szName), sizeof(_char) * MAX_PATH);
    loadStream.read(reinterpret_cast<char*>(&m_TransformationMatrix), sizeof(_float4x4));
    loadStream.read(reinterpret_cast<char*>(&m_CombinedTransformMatrix), sizeof(_float4x4));

    return S_OK;
}

CBone* CBone::Create(const aiNode* pNode, _int iParentIndex)
{
    CBone* pInstance = new CBone();

    if (FAILED(pInstance->Initialize(pNode, iParentIndex))) {
        MSG_BOX(TEXT("Failed to Create : CBone"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CBone* CBone::Create(ifstream& loadStream)
{
    CBone* pInstance = new CBone();

    if (FAILED(pInstance->LoadBone(loadStream))) {
        MSG_BOX(TEXT("Failed to Create : CBone"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CBone* CBone::Clone()
{
    return new CBone(*this);
}

void CBone::Free()
{
    __super::Free();
}
