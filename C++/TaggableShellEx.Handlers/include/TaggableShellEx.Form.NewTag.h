#pragma once
#include "dllmain.h"
#include "../src/resource.h"

class FormNewTag
{
public:
	FormNewTag(void);
	virtual ~FormNewTag(void);

	static LRESULT CALLBACK DlgProc(_In_  HWND hwnd,_In_  UINT uMsg,_In_  WPARAM wParam,_In_  LPARAM lParam);
};

