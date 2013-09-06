#pragma once
#include <Windows.h>  
#include "resource.h"

class MainForm
{
public:
	MainForm(void);
	virtual ~MainForm(void);

	HWND hdlg;
	HINSTANCE hApp;
	LRESULT CALLBACK DlgProc(_In_  HWND hwnd,_In_  UINT uMsg,_In_  WPARAM wParam,_In_  LPARAM lParam);

protected:

private:
	HRESULT LoadShellItems(void);
	HTREEITEM MainForm::InsertItem(HWND hwnd,const wchar_t* str, HTREEITEM parent, HTREEITEM insertAfter,int imageIndex, int selectedImageIndex,LPARAM pidl);
	int TreeViewNotify(WPARAM wParam, LPARAM lParam);
};

