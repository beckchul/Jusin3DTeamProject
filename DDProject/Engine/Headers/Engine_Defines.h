#pragma once
#ifndef Defines_h__
#define Defines_h__

#define EPSILON 0.00001f // 부동 소수점 오차를 잡아내자

#define PLANE_NEAR 0.2f
#define PLANE_FAR 10000.f

#include <d3d9.h>
#include <d3dx9.h>

#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>
#include <list>
#include <vector>
#include <array>
#include <string>
#include <map>
#include <unordered_map>
#include <tuple>
#include <assert.h>
#include <tchar.h>
#include <iostream>

using namespace std;

#include "Engine_Macro.h"
#include "Engine_Function.h"
#include "Engine_Functor.h"
#include "Engine_Enum.h"
#include "Engine_Typedef.h"
#include "Engine_Base.h"
#include "Engine_Struct.h"
#include "Engine_Constant.h"

// - STL 또는 D3D 구조체등을 dll프로젝트 클래스의 멤버로 사용할 때
// 해당 구조체는 _declspec(exportdll) 이 적용되어 있지 않기 때문에
// 경고창을 띄운다. 해당 경고창을 무시하기위한 구문 (warning C4251)
#pragma warning(disable: 4251)

// DirectShow warning disable
#pragma warning( disable : 4275)
#pragma warning( disable : 4995)
#pragma warning( disable : 4099)

#endif

#ifndef Server__
#define Server__
#endif

// 시연자
#ifndef Demonstration
#define Demonstration
#endif

// The Emperor
#ifdef Demonstration
	#ifndef Emperor
	#define Emperor
	#endif
#endif

#ifndef Lastman_Client // Bin.exe 게이머용
//#define Lastman_Client
#endif

// 시연자 제외 나.머.지
#ifndef BadComputer
//#define BadComputer
#endif

#ifdef _DEBUG
//#include "vld.h"
#endif
