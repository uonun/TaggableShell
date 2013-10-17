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

	// cached handles of controls. not need to call GetDlgItem again and again.
	HWND _hErrorInfo;
	HWND _hListTags;
	HWND _hListFiles;
	HWND _hEditCtlNewTag;
	HWND _hCheckAttachToFiles;
	HWND _hBuAdd;

	void InitText();
	void InitTagList(IImageList *imgList);
	void InitFileList(IImageList *imgList);

	COLORREF _msgColor;
	void ShowMsg(LPWSTR msg,COLORREF color = RGB(0,0,0));
	
	void LoadTags(void);
	void LoadSelectedFiles(void);
	void AddTag(void);
	BOOL IsNewTagOk(_Out_ wchar_t* key,_Out_  UINT & keyLength);

	void OnNewTagChanged(WPARAM wParam, LPARAM lParam);

	// handle WM_NOTIFY	
	void WM_NOTIFY_EDIT_TagWord(WPARAM & wParam, LPARAM & lParam);
	void WM_NOTIFY_LIST_Tags(WPARAM & wParam, LPARAM & lParam);
	void WM_NOTIFY_LIST_Files(WPARAM & wParam, LPARAM & lParam);

	// get values for each column in rows in list view control.
	void GetListColValue_Tags(NMLVDISPINFO* & plvdi);
	void GetListColValue_Files(NMLVDISPINFO* & plvdi);
	int GetImgIdxInList(LPWSTR & pszPath);
};

