#pragma once
#include "dllmain.h"

class ContextMenuHandler:
	public IShellExtInit,
	public IContextMenu
{
public:
	ContextMenuHandler(void);

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

private:
	~ContextMenuHandler(void);

	// IShellExtInit
	LPITEMIDLIST  m_pIDFolder;           //The folder's PIDL
	TCHAR          m_szFile[MAX_PATH];    //The file name
	IDataObject   *m_pDataObj;            //The IDataObject pointer
	HKEY           m_hRegKey;             //The file or folder's registry key

	// IUnknown
	long _cRef;
};