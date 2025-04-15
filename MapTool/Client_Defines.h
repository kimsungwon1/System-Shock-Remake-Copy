#pragma once

#include <process.h>
#include <Windows.h>
namespace Client
{
	const unsigned int			g_iWinSizeX = { 1280 };
	const unsigned int			g_iWinSizeY = { 720 };

	enum LEVELID { LEVEL_STATIC, LEVEL_TOOL, LEVEL_END };
}

extern HINSTANCE g_hInst;
extern HWND g_hWnd;

using namespace Client;
using namespace std;
