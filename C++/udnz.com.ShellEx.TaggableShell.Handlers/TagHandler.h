#pragma once
#include <windows.h>
#include <shlwapi.h>
#include <shlobj.h>
#include "dllmain.h"

class TagHandler:
	public IInitializeWithStream
{
public:
	TagHandler(void);

	// IUnknown
	IFACEMETHODIMP QueryInterface(REFIID riid, void ** ppv);

	IFACEMETHODIMP_(ULONG) AddRef();

	IFACEMETHODIMP_(ULONG) Release();

	// IInitializeWithStream
	STDMETHODIMP Initialize(IStream *pStream,DWORD grfMode);

private:
	~TagHandler(void);

	LPCITEMIDLIST  m_pIDFolder;           //The folder's PIDL
	TCHAR          m_szFile[MAX_PATH];    //The file name
	IDataObject   *m_pDataObj;            //The IDataObject pointer
	HKEY           m_hRegKey;             //The file or folder's registry key

	long _cRef;
	DWORD _grfMode;
	IStream *_pStream;
};