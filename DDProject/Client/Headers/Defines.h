#pragma once

#include "../Default/stdafx.h"

#include "Engine_Defines.h"

#include "Macro.h"
#include "Enum.h"
#include "Struct.h"
#include "Constant.h"

extern HWND g_hWnd;
extern HINSTANCE g_hInst;
extern int g_iClientIndex; // 현재 자기 클라이언트가 몇번인지
extern SCENEID g_eCurrentScene; // 현재 Scene
extern LASTMAN_PROGRESS g_eLastMan_Progress;
extern _int g_iRound;
extern bool g_bTextShow;