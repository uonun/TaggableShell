#pragma once
#include <Windows.h>   
#include <CommCtrl.h> //包含头文件     
#include "MainForm.h"
#include "Utils.h"
#include "resource.h"

// 导入静态库     
#pragma comment(lib, "Comctl32.lib")     
// 开启视觉效果 Copy from MSDN     
#pragma comment(linker,"\"/manifestdependency:type='win32' \
name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

LRESULT CALLBACK DlgProc(_In_  HWND hwnd,_In_  UINT uMsg,_In_  WPARAM wParam,_In_  LPARAM lParam);