#pragma once
#include "Base.h"

BEGIN(Engine)

class CAnimation : public CBase
{
private:
	CAnimation() = default;
	CAnimation(const CAnimation& Prototype);
	~CAnimation() = default;

public:
	HRESULT Initialize(const aiAnimation* pAnim, const class CModel* pModel);
	_bool Update_TransformationMatrices(vector<class CBone*>& vecRefBones, _bool isLoop, _float fTimeDelta, class CAnimation* pNextAnimation = nullptr);

	const vector<class CChannel*>& const Get_Channels() { return m_Channels; }
private:
	// �� �ִϸ��̼��� ����ϴ� ���� ����
	_uint		m_iNumChannels = { 0 };
	//���� ������ ����
	//Channel : ��. �ð��� ���� ���� ���� ����� ����.
	vector<class CChannel*>		m_Channels;
	// �ִϸ��̼� �����ϱ� ���� ��ü �Ÿ�
	_double		m_Duration = {};
	// �ִϸ��̼��� �ʴ� �ӵ�
	_double		m_SpeedPerSec = {};
	// m_CurrentTrackPosition += m_SpeedPerSec * fTimeDelta
	_double		m_CurrentTrackPosition = {};
	_double		m_dTrackOffset = { -1.0 };

	vector<_uint>		m_CurrentKeyFrameIndices = { 0 };

	_char m_szName[MAX_PATH] = {};

public:
	string Get_Name() const { return string(m_szName); }
	_double Get_SpeedPerSec() const { return m_SpeedPerSec; }
	void InitializeTrackPosition() { m_CurrentTrackPosition = 0.f; }
	_double Get_Duration() const { return m_Duration; }
	
public:
	HRESULT SaveAnim(ofstream& saveStream);
	HRESULT LoadAnim(ifstream& loadStream);
private:
	void SetOffset(_double dTrackOffset);
public:
	static CAnimation* Create(ifstream& loadStream);
	static CAnimation* Create(const aiAnimation* pAnim, const class CModel* pModel);
	CAnimation* Clone();
	void Free() override;
};

END