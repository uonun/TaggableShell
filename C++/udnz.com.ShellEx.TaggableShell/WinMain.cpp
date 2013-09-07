#include "WinMain.h"

int CALLBACK WinMain(     
	_In_  HINSTANCE hInstance,     
	_In_  HINSTANCE hPrevInstance,     
	_In_  LPSTR lpCmdLine,     
	_In_  int nCmdShow     
	)     
{   
	auto hwnd = GetDesktopWindow();
	auto f = MainForm::Instance();
	auto hdlg =	CreateDialog(hInstance,MAKEINTRESOURCE(IDD_MAINFORM),hwnd,(DLGPROC)DlgProc);	
	f->Init(hdlg,hInstance);

	if(f->hdlg!=NULL){
		ShowWindow(f->hdlg, SW_SHOW);

		auto fCaption = Utils::MyLoadString(IDS_FROMCAPTION);
		SetWindowText(f->hdlg,fCaption);
		
		// ¸üÐÂ´°¿Ú     
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
	auto f = MainForm::Instance();
	return f->DlgProc(hwnd,uMsg,wParam,lParam);
}