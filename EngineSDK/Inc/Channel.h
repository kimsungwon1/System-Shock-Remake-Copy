#pragma once
#include "Base.h"

BEGIN(Engine)

class CChannel : public CBase
{
private:
	CChannel();
	~CChannel() = default;

public:
	HRESULT Initialize(const aiNodeAnim* pAIChannel, const class CModel* pModel);
	void Update_TransformationMatrix(vector<class CBone*> Bones, _uint* pCurrentKeyFrameIndex, _double CurrentTrackPosition, class CAnimation* pNextAnimation);
	const _char* Get_Name() const { return m_szName; }
	const vector<KEYFRAME>& Get_KeyFrames() const { return m_KeyFrames; }

	void Set_Offset(_double dOffset);
public:
	HRESULT SaveChannel(ofstream& saveStream);
	HRESULT LoadChannel(ifstream& loadStream);
private:
	_char			m_szName[MAX_PATH] = {};
	_uint			m_iBoneIndex = { 0 };

	_uint			m_iNumKeyFrames = {  };
	vector<KEYFRAME>	m_KeyFrames;

	_double			m_dTrackPositionOffset;
public:
	static CChannel* Create(ifstream& loadStream);
	static CChannel* Create(const aiNodeAnim* pAIChannel, const class CModel* pModel);
	virtual void Free() override;
};
END
