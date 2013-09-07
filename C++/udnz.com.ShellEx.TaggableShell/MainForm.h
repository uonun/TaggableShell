#pragma once
#include <Windows.h>  
#include <ShlObj.h>
#include <shlwapi.h>
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
	HRESULT LoadShellItems(void);
	HTREEITEM MainForm::InsertItem(HWND hwnd,const wchar_t* str, HTREEITEM parent, HTREEITEM insertAfter,int imageIndex, int selectedImageIndex,LPARAM pidl);
	int TreeViewNotify(WPARAM wParam, LPARAM lParam);
	HRESULT MainForm::LoadSubItem(IShellFolder* sf,HWND tv,HTREEITEM parent,int deep = 1);
};

