#include "Animation.h"
#include "Channel.h"
#include "Bone.h"
#include "Model.h"

CAnimation::CAnimation(const CAnimation& Prototype)
    : m_iNumChannels { Prototype.m_iNumChannels }
    , m_Channels { Prototype.m_Channels }
    , m_Duration { Prototype.m_Duration }
    , m_SpeedPerSec{ Prototype.m_SpeedPerSec }
    , m_CurrentTrackPosition{ Prototype.m_CurrentTrackPosition }
    , m_CurrentKeyFrameIndices{ Prototype.m_CurrentKeyFrameIndices }
    , m_dTrackOffset{ Prototype.m_dTrackOffset }
{
    for (auto& pChannel : m_Channels)
        Safe_AddRef(pChannel);
    strcpy_s(m_szName, Prototype.m_szName);
}

HRESULT CAnimation::Initialize(const aiAnimation* pAnim, const CModel* pModel)
{
    m_Duration = pAnim->mDuration;
    m_SpeedPerSec = pAnim->mTicksPerSecond;

    m_iNumChannels = pAnim->mNumChannels;
    
    strcpy_s(m_szName, pAnim->mName.data);

    m_CurrentKeyFrameIndices.resize(m_iNumChannels);

    for (size_t i = 0; i < m_iNumChannels; i++)
    {
        CChannel* pChannel = CChannel::Create(pAnim->mChannels[i], pModel);
        if (pChannel == nullptr) {
            return E_FAIL;
        }

        m_Channels.emplace_back(pChannel);
    }

    return S_OK;
}

_bool CAnimation::Update_TransformationMatrices(vector<CBone*>& vecRefBones, _bool isLoop, _float fTimeDelta, CAnimation* pNextAnimation)
{
    if(m_CurrentTrackPosition == 0.f)
    {
        for (int i = 0; i < m_CurrentKeyFrameIndices.size(); i++) {
            m_CurrentKeyFrameIndices[i] = 0;
        }
    }
    // 현재 재생위치 계산
    m_CurrentTrackPosition += m_SpeedPerSec * fTimeDelta;

    if(pNextAnimation == nullptr)
    {
        SetOffset(-1.0);
        // m_CurrentTrackPosition을 초기화하는지 해야 한다.
        if (m_CurrentTrackPosition >= m_Duration && m_CurrentTrackPosition < m_Duration + CModel::GetAnimToAnimSpeed())
        {
            if (isLoop == false)
                return true;
            ////loop
            //else {
            //    pNextAnimation = this;
            //}
            //m_CurrentTrackPosition = 0.f;
            pNextAnimation = this;
        }
        else if (m_CurrentTrackPosition >= m_Duration + CModel::GetAnimToAnimSpeed())
        {
            return true;
        }

        //현재 재생위치에 맞게 현재 애니메이션이 컨트롤해야 할 뼈의 상태들을 갱신해줌
        _uint iChannelIndex = { 0 };
        for (auto& pChannel : m_Channels)
        {
            if(pNextAnimation == nullptr)
            {
                pChannel->Set_Offset(-1.0);
            }
            else {
                pChannel->Set_Offset(m_CurrentTrackPosition);
            }
            pChannel->Update_TransformationMatrix(vecRefBones, &m_CurrentKeyFrameIndices[iChannelIndex++], m_CurrentTrackPosition, pNextAnimation);
        }
    }
    else
    {
        SetOffset(m_CurrentTrackPosition);
        if (m_CurrentTrackPosition >= m_dTrackOffset + CModel::GetAnimToAnimSpeed()/*m_SpeedPerSec*/)
        {
            /*if (isLoop == false)
                return true;*/
            return true;
        }
        //현재 재생위치에 맞게 현재 애니메이션이 컨트롤해야 할 뼈의 상태들을 갱신해줌
        _uint iChannelIndex = { 0 };
        for (auto& pChannel : m_Channels)
        {
            pChannel->Set_Offset(m_CurrentTrackPosition);
            pChannel->Update_TransformationMatrix(vecRefBones, &m_CurrentKeyFrameIndices[iChannelIndex++], m_CurrentTrackPosition, pNextAnimation);
        }
    }

    

    return false;
}

HRESULT CAnimation::SaveAnim(ofstream& saveStream)
{
    saveStream.write(reinterpret_cast<const char*>(&m_Duration), sizeof(m_Duration));
    saveStream.write(reinterpret_cast<const char*>(&m_SpeedPerSec), sizeof(m_SpeedPerSec));

    saveStream.write(reinterpret_cast<const char*>(&m_iNumChannels), sizeof(m_iNumChannels));

    saveStream.write(reinterpret_cast<const char*>(m_szName), sizeof(_char) * MAX_PATH);
   
    saveStream.write(reinterpret_cast<const char*>(m_CurrentKeyFrameIndices.data()), 
        sizeof(_uint) * m_CurrentKeyFrameIndices.size());
    
    for (size_t i = 0; i < m_iNumChannels; i++)
    {
        m_Channels[i]->SaveChannel(saveStream);
    }

    saveStream.write(reinterpret_cast<const char*>(&m_CurrentTrackPosition), sizeof(_double));
    saveStream.write(reinterpret_cast<const char*>(&m_dTrackOffset), sizeof(_double));
    return S_OK;
}

HRESULT CAnimation::LoadAnim(ifstream& loadStream)
{
    loadStream.read(reinterpret_cast<char*>(&m_Duration), sizeof(m_Duration));
    loadStream.read(reinterpret_cast<char*>(&m_SpeedPerSec), sizeof(m_SpeedPerSec));

    loadStream.read(reinterpret_cast<char*>(&m_iNumChannels), sizeof(m_iNumChannels));

    loadStream.read(reinterpret_cast<char*>(m_szName), sizeof(_char) * MAX_PATH);

    m_CurrentKeyFrameIndices.resize(m_iNumChannels);
    _uint* cKeyFrameIndices = new _uint[m_iNumChannels];
    loadStream.read(reinterpret_cast<char*>(cKeyFrameIndices), sizeof(_uint) * m_iNumChannels);

    for (size_t i = 0; i < m_iNumChannels; i++)
    {
        m_CurrentKeyFrameIndices[i] = cKeyFrameIndices[i];
    }

    for (size_t i = 0; i < m_iNumChannels; i++)
    {
        CChannel* pInstance = CChannel::Create(loadStream);
        m_Channels.emplace_back(pInstance);
    }

    loadStream.read(reinterpret_cast<char*>(&m_CurrentTrackPosition), sizeof(_double));
    loadStream.read(reinterpret_cast<char*>(&m_dTrackOffset), sizeof(_double));
    Safe_Delete_Array(cKeyFrameIndices);
    return S_OK;
}

void CAnimation::SetOffset(_double dTrackOffset)
{
    if (dTrackOffset == -1.0) {
        m_dTrackOffset = -1.0;
    }
    else if(m_dTrackOffset == -1.0) {
        m_dTrackOffset = dTrackOffset;
    }
}

CAnimation* CAnimation::Create(ifstream& loadStream)
{
    CAnimation* pInstance = new CAnimation;

    if (FAILED(pInstance->LoadAnim(loadStream))) {
        MSG_BOX(TEXT("Failed to Create : CAnimation"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CAnimation* CAnimation::Create(const aiAnimation* pAnim, const CModel* pModel)
{
    CAnimation* pInstance = new CAnimation;

    if (FAILED(pInstance->Initialize(pAnim, pModel))) {
        MSG_BOX(TEXT("Failed to Create : CAnimation"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CAnimation* CAnimation::Clone()
{
    return new CAnimation(*this);
}

void CAnimation::Free()
{
    __super::Free();

    for (auto& pChannel : m_Channels)
        Safe_Release(pChannel);

    m_Channels.clear();
}
