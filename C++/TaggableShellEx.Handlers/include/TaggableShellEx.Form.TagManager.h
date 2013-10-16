#pragma once
#include "dllmain.h"
#include "TaggableShellEx.CHandler.h"

class FormTagManager
{
#pragma region singleton
public:
	static FormTagManager *instance () 
	{ 
		if (0 == p_instance_) { 
			static FormTagManager instance; 
			instance.AddRef();
			p_instance_ = &instance; 
		} 
		return p_instance_; 
	} 

private :
	FormTagManager();
	~FormTagManager();

	static FormTagManager *p_instance_; 
#pragma endregion

public:
	IFACEMETHODIMP_(ULONG) AddRef();
	IFACEMETHODIMP_(ULONG) Release();

	LRESULT CALLBACK DlgProc(_In_  HWND hwnd,_In_  UINT uMsg,_In_  WPARAM wParam,_In_  LPARAM lParam);

private :
	CHandler* _handler;
	HWND _hwnd;
	long _cRef;

	COLORREF _msgColor;
	void ShowMsg(LPWSTR msg,COLORREF color = RGB(0,0,0));
	void LoadTags(void);
	void LoadSelectedFiles(void);
	void AddTag(void);
	BOOL IsNewTagOk(_Out_ wchar_t* key,_Out_  UINT & keyLength, HWND editCtl = NULL);
};

