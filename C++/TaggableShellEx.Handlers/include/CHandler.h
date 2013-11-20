#pragma once
#include "dllmain.h"
#include "Taghelper.h"

class CHandler:
	public IContextMenu,		// ContextMenu
	public IShellPropSheetExt,	// PropertyPage
	public IShellExtInit		// ContextMenu, PropertyPage
{
public:
	CHandler(void);
	~CHandler(void);

	// IUnknown
	IFACEMETHODIMP QueryInterface(REFIID riid, void ** ppv);
	IFACEMETHODIMP_(ULONG) AddRef();
	IFACEMETHODIMP_(ULONG) Release();

	// IShellExtInit for ContextMenu, PropertyPage
	STDMETHODIMP Initialize(LPCITEMIDLIST pIDFolder, 
		IDataObject *pDataObj, 
		HKEY hRegKey);

	// IContextMenu
	STDMETHODIMP GetCommandString(UINT_PTR idCmd, UINT uFlags, UINT* pwReserved, LPSTR pszName, UINT cchMax);
	STDMETHODIMP InvokeCommand(LPCMINVOKECOMMANDINFO pCmdInfo);
	STDMETHODIMP QueryContextMenu(HMENU hmenu, UINT uMenuIndex, UINT uidFirstCmd, UINT uidLastCmd, UINT uFlags);

	// IShellPropSheetExt
	STDMETHODIMP AddPages(LPFNADDPROPSHEETPAGE lpfnAddPage, LPARAM lParam);
	STDMETHODIMP ReplacePage(UINT uPageID,LPFNADDPROPSHEETPAGE pfnReplacePage,LPARAM lParam){ return E_NOTIMPL; }

	CTaghelper* pTagHelper;
	UINT	   FileCount;

	HWND _hdlg;

private:

	// IUnknown
	long _cRef;

	// IShellExtInit
	LPITEMIDLIST  m_pIDFolder;           //The folder's PIDL
	TCHAR*        m_szFiles[MAXCOUNT_SELECTED_ITEM];    //The file names
	IDataObject   *m_pDataObj;            //The IDataObject pointer

	// IShellPropSheetExt
	UINT _dllRefCount;

	// IContextMenu
	HMENU _hSubmenu;

	BOOL _contextMenuSupposed;
};

LRESULT CALLBACK DlgProc_TagManager(_In_  HWND hwnd,_In_  UINT uMsg,_In_  WPARAM wParam,_In_  LPARAM lParam);
LRESULT CALLBACK DlgProc_About(_In_  HWND hwnd,_In_  UINT uMsg,_In_  WPARAM wParam,_In_  LPARAM lParam);

