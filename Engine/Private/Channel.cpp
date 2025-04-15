#include "Channel.h"
#include "Model.h"
#include "Animation.h"

CChannel::CChannel()
    : m_dTrackPositionOffset{ -1.0 }
{
}

HRESULT CChannel::Initialize(const aiNodeAnim* pAIChannel, const CModel* pModel)
{
    strcpy_s(m_szName, pAIChannel->mNodeName.data);

    m_iBoneIndex = pModel->Get_BoneIndex(m_szName);

    m_iNumKeyFrames = max(pAIChannel->mNumScalingKeys, pAIChannel->mNumRotationKeys);
    m_iNumKeyFrames = max(m_iNumKeyFrames, pAIChannel->mNumPositionKeys);

    _float3 vScale = {};
    _float4 vRotation = {};
    _float3 vTranslation = {};

    for (size_t i = 0; i < m_iNumKeyFrames; i++)
    {
        KEYFRAME KeyFrame{};

        if (pAIChannel->mNumScalingKeys > i)
        {
            memcpy(&vScale, &pAIChannel->mScalingKeys[i].mValue, sizeof(_float3));
            KeyFrame.TrackPosition = pAIChannel->mScalingKeys[i].mTime;
        }

        if (pAIChannel->mNumRotationKeys > i)
        {
            vRotation.x = pAIChannel->mRotationKeys[i].mValue.x;
            vRotation.y = pAIChannel->mRotationKeys[i].mValue.y;
            vRotation.z = pAIChannel->mRotationKeys[i].mValue.z;
            vRotation.w = pAIChannel->mRotationKeys[i].mValue.w;
            KeyFrame.TrackPosition = pAIChannel->mRotationKeys[i].mTime;
        }

        if (pAIChannel->mNumPositionKeys > i)
        {
            memcpy(&vTranslation, &pAIChannel->mPositionKeys[i].mValue, sizeof(_float3));
            KeyFrame.TrackPosition = pAIChannel->mPositionKeys[i].mTime;
        }

        KeyFrame.vScale = vScale;
        KeyFrame.vRotation = vRotation;
        KeyFrame.vTranslation = vTranslation;

        m_KeyFrames.push_back(KeyFrame);
    }

    return S_OK;
}

void CChannel::Update_TransformationMatrix(vector<CBone*> Bones, _uint* pCurrentKeyFrameIndex, _double CurrentTrackPosition, class CAnimation* pNextAnimation)
{
    if (0.0 == CurrentTrackPosition)
        *pCurrentKeyFrameIndex = 0;

    KEYFRAME LastKeyFrame = m_KeyFrames.back();

    _vector vScale, vRotation, vTranslation;

    // 보간 필요없는 경우
    if ((CurrentTrackPosition >= LastKeyFrame.TrackPosition && pNextAnimation == nullptr) || (pNextAnimation != nullptr && CurrentTrackPosition >= m_dTrackPositionOffset + CModel::GetAnimToAnimSpeed()))
    {
        vScale = XMLoadFloat3(&LastKeyFrame.vScale);
        vRotation = XMLoadFloat4(&LastKeyFrame.vRotation);
        vTranslation = XMVectorSetW(XMLoadFloat3(&LastKeyFrame.vTranslation), 1.f);
    }
    //다음 애니메이션 보간
    else if (pNextAnimation != nullptr && CurrentTrackPosition >= m_dTrackPositionOffset && CurrentTrackPosition < m_dTrackPositionOffset + CModel::GetAnimToAnimSpeed())
    {
        for (auto* pChannel : pNextAnimation->Get_Channels()) {
            if (0 == strcmp(pChannel->Get_Name(), m_szName)) {
                auto destFrame = pChannel->Get_KeyFrames()[0];
                KEYFRAME LastLastFrame;
                if (m_dTrackPositionOffset >= LastKeyFrame.TrackPosition) { 
                    LastLastFrame = LastKeyFrame;
                }
                else {
                    LastLastFrame = m_KeyFrames[*pCurrentKeyFrameIndex];
                }
                _vector vSourScale = XMLoadFloat3(&LastLastFrame.vScale);
                _vector vDestScale = XMLoadFloat3(&destFrame.vScale);

                _vector vSourRotation = XMLoadFloat4(&LastLastFrame.vRotation);
                _vector vDestRotation = XMLoadFloat4(&destFrame.vRotation);

                _vector vSourTranslation = XMVectorSetW(XMLoadFloat3(&LastLastFrame.vTranslation), 1.f);
                _vector vDestTranslation = XMVectorSetW(XMLoadFloat3(&destFrame.vTranslation), 1.f);

                _double Ratio = (CurrentTrackPosition - m_dTrackPositionOffset)
                    / CModel::GetAnimToAnimSpeed();

                if (CModel::GetAnimToAnimSpeed() == 0.f) {
                    vScale = vDestScale;
                    vRotation = vDestRotation;
                    vTranslation = vDestTranslation;
                    continue;
                }

                vScale = XMVectorLerp(vSourScale, vDestScale, (_float)Ratio);
                vRotation = XMQuaternionSlerp(vSourRotation, vDestRotation, (_float)Ratio);
                vTranslation = XMVectorLerp(vSourTranslation, vDestTranslation, (_float)Ratio);
            }
        }
    }
    // 보간 필요한 대부분의 경우
    else
    {
        if (*pCurrentKeyFrameIndex + 1 >= m_KeyFrames.size()) {
            --(*pCurrentKeyFrameIndex);
        }

        while (CurrentTrackPosition >= m_KeyFrames[(*pCurrentKeyFrameIndex) + 1].TrackPosition)
        {
            ++(*pCurrentKeyFrameIndex);
            if (*pCurrentKeyFrameIndex + 1 >= m_KeyFrames.size()) {
                (*pCurrentKeyFrameIndex)--;
                break;
            }
        }
        

        _vector vSourScale = XMLoadFloat3(&m_KeyFrames[(*pCurrentKeyFrameIndex)].vScale);
        _vector vDestScale = XMLoadFloat3(&m_KeyFrames[(*pCurrentKeyFrameIndex) + 1].vScale);

        _vector vSourRotation = XMLoadFloat4(&m_KeyFrames[(*pCurrentKeyFrameIndex)].vRotation);
        _vector vDestRotation = XMLoadFloat4(&m_KeyFrames[(*pCurrentKeyFrameIndex + 1)].vRotation);

        _vector vSourTranslation = XMVectorSetW(XMLoadFloat3(&m_KeyFrames[(*pCurrentKeyFrameIndex)].vTranslation), 1.f);
        _vector vDestTranslation = XMVectorSetW(XMLoadFloat3(&m_KeyFrames[(*pCurrentKeyFrameIndex + 1)].vTranslation), 1.f);

        _double Ratio = (CurrentTrackPosition - m_KeyFrames[(*pCurrentKeyFrameIndex)].TrackPosition)
            / (m_KeyFrames[(*pCurrentKeyFrameIndex) + 1].TrackPosition - m_KeyFrames[*pCurrentKeyFrameIndex].TrackPosition);

        vScale = XMVectorLerp(vSourScale, vDestScale, (_float)Ratio);
        vRotation = XMQuaternionSlerp(vSourRotation, vDestRotation, (_float)Ratio);
        vTranslation = XMVectorLerp(vSourTranslation, vDestTranslation, (_float)Ratio);
    }

    _matrix TransformationMatrix = XMMatrixAffineTransformation(vScale, XMVectorSet(0.f, 0.f, 0.f, 1.f), vRotation, vTranslation);

    Bones[m_iBoneIndex]->Set_TransformationMatrix(TransformationMatrix);
}

void CChannel::Set_Offset(_double dOffset)
{
    if (dOffset == -1.0) {
        m_dTrackPositionOffset = -1.0;
    }
    else if (m_dTrackPositionOffset == -1.0) {
        m_dTrackPositionOffset = dOffset;
    }
}

HRESULT CChannel::SaveChannel(ofstream& saveStream)
{
    saveStream.write(reinterpret_cast<const char*>(m_szName), sizeof(_char) * MAX_PATH);
    saveStream.write(reinterpret_cast<const char*>(&m_iBoneIndex), sizeof(_uint));
    saveStream.write(reinterpret_cast<const char*>(&m_iNumKeyFrames), sizeof(_uint));

    for (size_t i = 0; i < m_iNumKeyFrames; i++)
    {
        saveStream.write(reinterpret_cast<const char*>(&m_KeyFrames[i]), sizeof(KEYFRAME));
    }

    saveStream.write(reinterpret_cast<const char*>(&m_dTrackPositionOffset), sizeof(_double));

    return S_OK;
}

HRESULT CChannel::LoadChannel(ifstream& loadStream)
{
    loadStream.read(reinterpret_cast<char*>(m_szName), sizeof(_char) * MAX_PATH);
    loadStream.read(reinterpret_cast<char*>(&m_iBoneIndex), sizeof(_uint));
    loadStream.read(reinterpret_cast<char*>(&m_iNumKeyFrames), sizeof(_uint));

    for (size_t i = 0; i < m_iNumKeyFrames; i++)
    {
        KEYFRAME newKeyFrame;
        loadStream.read(reinterpret_cast<char*>(&newKeyFrame), sizeof(KEYFRAME));
        m_KeyFrames.push_back(newKeyFrame);
    }
    loadStream.read(reinterpret_cast<char*>(&m_dTrackPositionOffset), sizeof(_double));

    return S_OK;
}

CChannel* CChannel::Create(ifstream& loadStream)
{
    CChannel* pInstance = new CChannel;
    if (FAILED(pInstance->LoadChannel(loadStream))) {
        MSG_BOX(TEXT("Failed to Create: CChannel"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CChannel* CChannel::Create(const aiNodeAnim* pAIChannel, const CModel* pModel)
{
    CChannel* pInstance = new CChannel();

    if (FAILED(pInstance->Initialize(pAIChannel, pModel))) {
        MSG_BOX(TEXT("Failed to Create: CChannel"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CChannel::Free()
{
    __super::Free();
}
