#pragma once
#include "dllmain.h"
#include "TaggableShellEx.Taghelper.h"

// CMD-IDs for ContextMenu, must be consecutive from 0x1
#define CMD_NEWTAG						0x1
#define CMD_SETTINGS					0x2
#define CMD_ABOUT						0x3

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
	UINT _dllRefCount;

	// IContextMenu
	HMENU _hSubmenu;

	CTaghelper _tagHelper;
	IShellItem* _targetFile; 
};