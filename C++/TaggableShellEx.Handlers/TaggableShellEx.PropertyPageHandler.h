#pragma once
#include <windows.h>
#include <shlwapi.h>
#include <shlobj.h>
#include "dllmain.h"

class PropertyPageHandler:
	public IShellExtInit,
	public IShellPropSheetExt
{
public:
	PropertyPageHandler(void);

	// IUnknown
	IFACEMETHODIMP QueryInterface(REFIID riid, void ** ppv);

	IFACEMETHODIMP_(ULONG) AddRef();

	IFACEMETHODIMP_(ULONG) Release();

	// IShellExtInit
	STDMETHODIMP Initialize(LPCITEMIDLIST pIDFolder, 
                                   IDataObject *pDataObj, 
                                   HKEY hRegKey);

	// IShellPropSheetExt
	STDMETHODIMP AddPages(LPFNADDPROPSHEETPAGE lpfnAddPage, LPARAM lParam);
	STDMETHODIMP ReplacePage(UINT uPageID,LPFNADDPROPSHEETPAGE pfnReplacePage,LPARAM lParam){ return E_NOTIMPL; }

private:
	~PropertyPageHandler(void);

	// IShellExtInit
	LPITEMIDLIST  m_pIDFolder;           //The folder's PIDL
	TCHAR          m_szFile[MAX_PATH];    //The file name
	IDataObject   *m_pDataObj;            //The IDataObject pointer
	HKEY           m_hRegKey;             //The file or folder's registry key

	// IUnknown
	long _cRef;


	// IInitializeWithStream
	DWORD _grfMode;
	IStream *_pStream;

	// IShellPropSheetExt
	UINT g_DllRefCount;
};