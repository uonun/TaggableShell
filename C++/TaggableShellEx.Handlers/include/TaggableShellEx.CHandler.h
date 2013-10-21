#pragma once
#include "dllmain.h"
#include "TaggableShellEx.Taghelper.h"

class CHandler:
	public IContextMenu,		// ContextMenu
	public IShellPropSheetExt,	// PropertyPage
	public IShellExtInit
{
public:
	CHandler(void);
	~CHandler(void);

	// IUnknown
	IFACEMETHODIMP QueryInterface(REFIID riid, void ** ppv);
	IFACEMETHODIMP_(ULONG) AddRef();
	IFACEMETHODIMP_(ULONG) Release();

	// IShellExtInit
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

	CTaghelper TagHelper;
	UINT	   FileCount;

	HWND _hdlg;

private:

	// IUnknown
	long _cRef;

	// IShellExtInit
	LPITEMIDLIST  m_pIDFolder;           //The folder's PIDL
	TCHAR*        m_szFiles[MAXCOUNT_ITEM];    //The file names
	IDataObject   *m_pDataObj;            //The IDataObject pointer

	// IShellPropSheetExt
	UINT _dllRefCount;

	// IContextMenu
	HMENU _hSubmenu;	
};