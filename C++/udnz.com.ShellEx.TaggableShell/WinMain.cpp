#include <Windows.h>   
#include <CommCtrl.h> //包含头文件     
#include "MainForm.h"
#include "WinMain.h"

// 导入静态库     
#pragma comment(lib, "Comctl32.lib")     
// 开启视觉效果 Copy from MSDN     
#pragma comment(linker,"\"/manifestdependency:type='win32' \
name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")


int CALLBACK WinMain(     
	_In_  HINSTANCE hInstance,     
	_In_  HINSTANCE hPrevInstance,     
	_In_  LPSTR lpCmdLine,     
	_In_  int nCmdShow     
	)     
{   
	auto hwnd = GetDesktopWindow();
	f = MainForm();
	f.hApp = hInstance;
	f.hdlg = CreateDialog(hInstance,MAKEINTRESOURCE(IDD_MAINFORM),hwnd,(DLGPROC)DlgProc);	

	if(f.hdlg!=NULL){
		ShowWindow(f.hdlg, SW_SHOW);

		TCHAR fCaption[160];
		LoadString(f.hApp,IDS_FROMCAPTION,fCaption,sizeof(fCaption)/sizeof(TCHAR));
		SetWindowText(f.hdlg,fCaption);

		// 更新窗口     
		UpdateWindow(hwnd);

		MSG msg;     
		while(GetMessage(&msg, NULL, 0, 0))     
		{     
			TranslateMessage(&msg);     
			DispatchMessage(&msg);     
		}
	}else{
		DWORD e = GetLastError();
		MessageBox(hwnd,L"Can not create window",L"Error",MB_OK);
	}

	return 0;
}

LRESULT CALLBACK DlgProc(_In_  HWND hwnd,_In_  UINT uMsg,_In_  WPARAM wParam,_In_  LPARAM lParam){
	return f.DlgProc(hwnd,uMsg,wParam,lParam);
}