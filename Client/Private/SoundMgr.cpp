#include "stdafx.h"
#include "SoundMgr.h"
#include <io.h>
#include <cstring>

using namespace FMOD;

CSoundMgr* CSoundMgr::m_pInstance = nullptr;
CSoundMgr::CSoundMgr()
{
	m_pSystem = nullptr; 
}


CSoundMgr::~CSoundMgr()
{
	Release(); 
}

void CSoundMgr::Initialize()
{
	// 사운드를 담당하는 대표객체를 생성하는 함수
	FMOD_RESULT result = System_Create(&m_pSystem);
	
	// 1. 시스템 포인터, 2. 사용할 가상채널 수 , 초기화 방식) 
	//FMOD_System_Init(m_pSystem, 32, FMOD_INIT_NORMAL, NULL);
	m_pSystem->init(32, FMOD_INIT_NORMAL, NULL);

	string str = FMOD_ErrorString(result);

	//LoadSoundFile(); 
}
void CSoundMgr::Release()
{
	for (auto& Mypair : m_mapSound)
	{
		delete[] Mypair.first;
		//FMOD_Sound_Release(Mypair.second);
		Mypair.second->release();
	}
	m_mapSound.clear(); 

	//FMOD_System_Release(m_pSystem);
	//FMOD_System_Close(m_pSystem);
	m_pSystem->release();
	m_pSystem->close();
}

void CSoundMgr::PlaySound(TCHAR * pSoundKey, float fVolume)
{
	map<TCHAR*, Sound*>::iterator iter; 

	// iter = find_if(m_mapSound.begin(), m_mapSound.end(), CTag_Finder(pSoundKey));
	iter = find_if(m_mapSound.begin(), m_mapSound.end(), 
		[&](auto& iter)->bool 
	{
		return !lstrcmp(pSoundKey, iter.first);
	});
	
	if (iter == m_mapSound.end())
		return;

	_bool bPlay = FALSE; 
	curChannelInd++;
	if (curChannelInd >= (int)MAXCHANNEL) {
		curChannelInd = 1;
	}

	//if (FMOD_Channel_IsPlaying(m_pChannelArr[curChannelInd], &bPlay))
	{
		//FMOD_System_PlaySound(m_pSystem, FMOD_CHANNEL_FREE, iter->second, FALSE, &m_pChannelArr[curChannelInd]);
	}

	//FMOD_Channel_SetVolume(m_pChannelArr[curChannelInd], fVolume);

	//FMOD_System_Update(m_pSystem);
	if (m_pChannelArr[curChannelInd]->isPlaying(&bPlay)) {
		m_pSystem->playSound(iter->second, NULL, FALSE, &m_pChannelArr[curChannelInd]);
	}
	m_pChannelArr[curChannelInd]->setMode(FMOD_LOOP_OFF);
	m_pChannelArr[curChannelInd]->setVolume(fVolume);

	m_pSystem->update();
}

void CSoundMgr::PlayBGM(const TCHAR * pSoundKey, float fVolume)
{
	map<TCHAR*, Sound*>::iterator iter;

	// iter = find_if(m_mapSound.begin(), m_mapSound.end(), CTag_Finder(pSoundKey));
	iter = find_if(m_mapSound.begin(), m_mapSound.end(), [&](auto& iter)->bool
	{
		return !lstrcmp(pSoundKey, iter.first);
	});
	
	if (iter == m_mapSound.end())
		return;
	m_pChannelArr[SOUND_BGM]->stop();
	//FMOD_System_PlaySound(m_pSystem, FMOD_CHANNEL_FREE, iter->second, FALSE, &m_pChannelArr[SOUND_BGM]);
	m_pSystem->playSound(iter->second, NULL, FALSE, &m_pChannelArr[SOUND_BGM]);
	//FMOD_Channel_SetMode(m_pChannelArr[SOUND_BGM], FMOD_LOOP_NORMAL);
	m_pChannelArr[SOUND_BGM]->setMode(FMOD_LOOP_NORMAL);
	//FMOD_Channel_SetVolume(m_pChannelArr[SOUND_BGM], fVolume);
	m_pChannelArr[SOUND_BGM]->setVolume(fVolume);
	//FMOD_System_Update(m_pSystem);
	m_pSystem->update();
}

void CSoundMgr::StopSound(CHANNELID eID)
{
	//FMOD_Channel_Stop(m_pChannelArr[eID]);
	m_pChannelArr[eID]->stop();
}

void CSoundMgr::StopAll()
{
	for (int i = 0; i < MAXCHANNEL; ++i)
	{
		m_pChannelArr[i]->stop();
	}
		//FMOD_Channel_Stop(m_pChannelArr[i]);
}

void CSoundMgr::SetChannelVolume(CHANNELID eID, float fVolume)
{
	//FMOD_Channel_SetVolume(m_pChannelArr[eID], fVolume);
	m_pChannelArr[eID]->setVolume(fVolume);

	//FMOD_System_Update(m_pSystem);
	m_pSystem->update();
}

void CSoundMgr::LoadSoundFile()
{
	// _finddata_t : <io.h>에서 제공하며 파일 정보를 저장하는 구조체
	_finddata_t fd; 
	memset(&fd, 0, sizeof(_finddata_t));
	// _findfirst : <io.h>에서 제공하며 사용자가 설정한 경로 내에서 가장 첫 번째 파일을 찾는 함수
	long handle = _findfirst("../Sound/*.*", &fd);

	if (handle == -1)
		return; 

	int iResult = 0; 

	// D:\유준환\145기\DefaultWindow\Sound	: 절대 경로

	char szCurPath[128] = "../Sound/";	 // 상대 경로
	char szFullPath[128] = ""; 

	while (iResult != -1)
	{
		strcpy_s(szFullPath, szCurPath); 
		
		// "../Sound/" + "Success.wav"
		strcat_s(szFullPath, fd.name);
		// "../Sound/Success.wav"

		Sound* pSound = nullptr; 

		if (strcmp(fd.name, ".") == 0 || strcmp(fd.name, "..") == 0) {
			iResult = _findnext(handle, &fd);
			continue;
		}

		//FMOD_RESULT eRes = FMOD_System_CreateSound(m_pSystem, szFullPath, FMOD_HARDWARE, 0, &pSound);
		FMOD_RESULT eRes = m_pSystem->createSound(szFullPath, FMOD_DEFAULT, NULL, &pSound);

		if (eRes == FMOD_OK)
		{
			int iLength = strlen(fd.name) + 1; 

			TCHAR* pSoundKey = new TCHAR[iLength];
			ZeroMemory(pSoundKey, sizeof(TCHAR) * iLength);

			// 아스키 코드 문자열을 유니코드 문자열로 변환시켜주는 함수
			MultiByteToWideChar(CP_ACP, 0, fd.name, iLength, pSoundKey, iLength);

			m_mapSound.emplace(pSoundKey, pSound);
		}
		//_findnext : <io.h>에서 제공하며 다음 위치의 파일을 찾는 함수, 더이상 없다면 -1을 리턴
		iResult = _findnext(handle, &fd);
	}

	//FMOD_System_Update(m_pSystem);
	m_pSystem->update();


	_findclose(handle);
}

HRESULT CSoundMgr::LoadSoundFile(const _char* strFileName)
{
	Sound* pSound = nullptr;

	FMOD_RESULT eRes = m_pSystem->createSound(strFileName, FMOD_LOOP_NORMAL | FMOD_DEFAULT, NULL, &pSound);

	if (eRes == FMOD_OK)
	{
		_char szFileName[MAX_PATH];

		_splitpath_s(strFileName, nullptr, 0, nullptr, 0, szFileName, MAX_PATH, nullptr, 0);

		int iLength = strlen(szFileName) + 1;

		TCHAR* pSoundKey = new TCHAR[iLength];
		ZeroMemory(pSoundKey, sizeof(TCHAR) * iLength);

		// 아스키 코드 문자열을 유니코드 문자열로 변환시켜주는 함수
		MultiByteToWideChar(CP_ACP, 0, szFileName, iLength, pSoundKey, iLength);

		m_mapSound.emplace(pSoundKey, pSound);
	}
	else {
		return E_FAIL;
	}
	m_pSystem->update();
}
