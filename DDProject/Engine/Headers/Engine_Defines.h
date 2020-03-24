#pragma once
#ifndef Defines_h__
#define Defines_h__

#define EPSILON 0.00001f // �ε� �Ҽ��� ������ ��Ƴ���

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

// - STL �Ǵ� D3D ����ü���� dll������Ʈ Ŭ������ ����� ����� ��
// �ش� ����ü�� _declspec(exportdll) �� ����Ǿ� ���� �ʱ� ������
// ���â�� ����. �ش� ���â�� �����ϱ����� ���� (warning C4251)
#pragma warning(disable: 4251)

// DirectShow warning disable
#pragma warning( disable : 4275)
#pragma warning( disable : 4995)
#pragma warning( disable : 4099)

#endif

#ifndef Server__
#define Server__
#endif

// �ÿ���
#ifndef Demonstration
#define Demonstration
#endif

// The Emperor
#ifdef Demonstration
	#ifndef Emperor
	#define Emperor
	#endif
#endif

#ifndef Lastman_Client // Bin.exe ���̸ӿ�
//#define Lastman_Client
#endif

// �ÿ��� ���� ��.��.��
#ifndef BadComputer
//#define BadComputer
#endif

#ifdef _DEBUG
//#include "vld.h"
#endif
