#pragma once

#include "Base.h"
#include "fmod.h"
#include "fmod.hpp"
#include "fmod_errors.h"
#pragma comment(lib, "fmodex_vc.lib")

class CSoundMgr
{
	enum CHANNELID
	{
		SOUND_BGM,
		MAXCHANNEL = 32
	};
public:
	static CSoundMgr* Get_Instance()
	{
		if (nullptr == m_pInstance)
			m_pInstance = new CSoundMgr; 

		return m_pInstance; 
	}
	static void Destroy_Instance()
	{
		if (m_pInstance)
		{
			delete m_pInstance; 
			m_pInstance = nullptr; 
		}
	}

private:
	CSoundMgr();
	~CSoundMgr();

public:
	void Initialize(); 
	void Release(); 

public:
	void PlaySound(TCHAR* pSoundKey, float fVolume);
	void PlayBGM(const TCHAR* pSoundKey, float fVolume);
	void StopSound(CHANNELID eID);
	void StopAll();
	void SetChannelVolume(CHANNELID eID, float fVolume);

private:
	void LoadSoundFile(); 
public:
	HRESULT LoadSoundFile(const _char* strFileName);
private:
	int curChannelInd = 1;

	static CSoundMgr* m_pInstance; 

	// 사운드 리소스 정보를 갖는 객체 
	map<TCHAR*, FMOD::Sound*> m_mapSound; 
	
	// FMOD_CHANNEL : 재생하고 있는 사운드를 관리할 객체 
	FMOD::Channel* m_pChannelArr[MAXCHANNEL]; 

	// 사운드 ,채널 객체 및 장치를 관리하는 객체 
	FMOD::System* m_pSystem; 
};

