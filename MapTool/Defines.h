#pragma once


#include <d3d11.h>
#include <tchar.h>

#include "ImguiDesign.h"

//#include "Effects11\d3dx11effect.h"
//#include "DirectXTK\DDSTextureLoader.h"
//#include "DirectXTK\WICTextureLoader.h"
//
//#include "assimp\scene.h"
//#include "assimp\Importer.hpp"
//#include "assimp\postprocess.h"
//
//#include <DirectXCollision.h>
//#include <DirectXMath.h>
//#include <d3dcompiler.h>
//
//using namespace DirectX;

#include <map>
#include <list>
#include <vector>
#include <algorithm>
#include <string>

using namespace std;

#include "Enums.h"
#include "Client_Defines.h"

#ifdef _DEBUG

#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

#ifndef DBG_NEW 

#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ ) 
#define new DBG_NEW 

#endif

#endif // _DEBUG