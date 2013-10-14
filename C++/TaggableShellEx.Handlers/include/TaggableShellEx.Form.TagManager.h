#pragma once
#include "dllmain.h"
#include "TaggableShellEx.CHandler.h"

class FormTagManager
{
public:
	FormTagManager(void);
	virtual ~FormTagManager(void);

	LRESULT CALLBACK DlgProc(_In_  HWND hwnd,_In_  UINT uMsg,_In_  WPARAM wParam,_In_  LPARAM lParam);

private :
	CHandler* _handler;
	HWND _hwnd;

	void LoadTags(void);
};

