#pragma once
#include "dllmain.h"
#include "Taghelper.h"

class CHandler:
	public IContextMenu,		// ContextMenu
	public IShellPropSheetExt,	// PropertyPage
	public IShellExtInit
{
public:
	CHandler(void);

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

private:
	~CHandler(void);

	// IUnknown
	long _cRef;

	// IShellExtInit
	LPITEMIDLIST  m_pIDFolder;           //The folder's PIDL
	TCHAR          m_szFile[MAX_PATH];    //The file name
	IDataObject   *m_pDataObj;            //The IDataObject pointer
	HKEY           m_hRegKey;             //The file or folder's registry key

	// IShellPropSheetExt
	UINT g_DllRefCount;

	// ContextMenuHandler
	CTaghelper _tagHelper;
};