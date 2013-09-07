#pragma once
#include <Windows.h>  
#include <WindowsX.h>
#include <ShlObj.h>
#include <shlwapi.h>
#include <commoncontrols.h>
#include <process.h>
#include <string>
//#include <sstream>
#include "WinMain.h"
using namespace std;

typedef LRESULT (*pDlgProc)(_In_  HWND hwnd,_In_  UINT uMsg,_In_  WPARAM wParam,_In_  LPARAM lParam);
typedef struct tagTVITEMDATA {
  bool bExpanded;
  IShellFolder *pShellFolder;
}TVITEMDATA,*LPTVITEMDATA;

class MainForm
{
public:
	virtual ~MainForm(void);

	static MainForm* Instance();
	HWND hdlg;
	HINSTANCE hApp;
	LRESULT CALLBACK DlgProc(_In_  HWND hwnd,_In_  UINT uMsg,_In_  WPARAM wParam,_In_  LPARAM lParam);
	void Init(HWND hdlg,HINSTANCE hApp);

protected:

private:
	MainForm(void);
	static MainForm* _instance;

	void GetDisplayName(IShellFolder* sf,LPITEMIDLIST pidlItems,_In_ UINT pszDisplayNameBuffSize,_Out_ LPWSTR pszDisplayName);

	// In C++ you must employ a free (C) function or a static
	// class member function as the thread entry-point-function.
	// Furthermore, _beginthreadex() demands that the thread
	// entry function signature take a single (void*) and returned
	// an unsigned.
	static unsigned __stdcall ThreadStaticEntryPoint(void * pThis);

	HRESULT LoadShellItems(void);
	HRESULT LoadImageList(void);
	// get image index in system image list.
	int GetImgIdxInList(LPCTSTR pszPath);

	HTREEITEM MainForm::InsertItem(HWND hwnd,const LPWSTR str, HTREEITEM parent, HTREEITEM insertAfter,int imageIndex, int selectedImageIndex,LPARAM pidl);
	HRESULT MainForm::LoadSubItem(IShellFolder* sf,HWND tv,HTREEITEM parent,int deep = 1);
	int TreeViewNotify(WPARAM wParam, LPARAM lParam);
};

