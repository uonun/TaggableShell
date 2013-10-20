#pragma once
#include "resource.h"
#include "../include/dllmain.h"
#include "../include/TaggableShellEx.CHandler.h"

CHandler::CHandler() : 
	_cRef(1) // IUnknown
	,m_pIDFolder(NULL),m_pDataObj(NULL)	// IShellExtInit
	,_dllRefCount(0) // IShellPropSheetExt
	,_hSubmenu(NULL) // IContextMenu
	,FileCount(0)
{
	::PrintLog(L"CHandler.ctor");

	for (UINT i = 0; i < FileCount; i++)
	{
		m_szFiles[i] = NULL;
	}

	DllAddRef();
}

CHandler::~CHandler(void)
{   
	::PrintLog(L"CHandler.~ctor");

	for (UINT i = 0; i < FileCount; i++)
	{
		if (m_szFiles != NULL && m_szFiles[i] != NULL){
			delete m_szFiles[i];
			m_szFiles[i] = NULL;
		}
	}

	DllRelease();
}

HRESULT CHandler_CreateInstance(REFIID riid, void **ppv)
{
	::PrintLog(L"CHandler.ContextMenuHandler_CreateInstance");
	CHandler *pNew = new(std::nothrow) CHandler;
	HRESULT hr = pNew ? S_OK : E_OUTOFMEMORY;
	if (pNew)
	{
		hr = pNew->QueryInterface(riid, ppv);
		pNew->Release();
	}
	return hr;
}

// IUnknown
IFACEMETHODIMP CHandler::QueryInterface(REFIID riid, void ** ppv)
{
	static const QITAB qit[] =
	{
		QITABENT(CHandler, IShellExtInit),
		QITABENT(CHandler, IContextMenu),
		QITABENT(CHandler, IShellPropSheetExt),
		{0},
	};
	return QISearch(this, qit, riid, ppv);
}

IFACEMETHODIMP_(ULONG) CHandler::AddRef()
{
	return InterlockedIncrement(&_cRef);
}

IFACEMETHODIMP_(ULONG) CHandler::Release()
{
	long cRef = InterlockedDecrement(&_cRef);
	if (cRef == 0)
	{
		delete this;
	}
	return cRef;
}

// http://msdn.microsoft.com/en-us/library/windows/desktop/cc144067(v=vs.85).aspx
// http://msdn.microsoft.com/en-us/library/cc144067%28v=VS.85%29.aspx
HRESULT CHandler::Initialize(LPCITEMIDLIST pIDFolder, 
							 IDataObject *pDataObj, 
							 HKEY hRegKey) 
{
	HRESULT hr = S_FALSE;

	// If Initialize has already been called, release the old PIDL
	if(m_pIDFolder!=NULL){
		ILFree(m_pIDFolder);
		m_pIDFolder = NULL;
	}

	// If Initialize has already been called, release the old
	// IDataObject pointer.
	if (m_pDataObj)
	{ 
		m_pDataObj->Release(); 
	}

	//Store the new PIDL.
	if(pIDFolder)
	{
		m_pIDFolder = ILClone(pIDFolder);
	}

	// If a data object pointer was passed in, save it and
	// extract the file name. 
	if (pDataObj) 
	{ 
		m_pDataObj = pDataObj; 
		pDataObj->AddRef(); 

		STGMEDIUM   medium;
		FORMATETC   fe = {CF_HDROP, NULL, DVASPECT_CONTENT, -1, TYMED_HGLOBAL};

		if(SUCCEEDED(m_pDataObj->GetData(&fe, &medium)))
		{
			// Get the count of files dropped.
			FileCount = DragQueryFile((HDROP)medium.hGlobal, (UINT)-1, NULL, 0);

			if( FileCount > 0 )
			{
				for (UINT i = 0; i < FileCount; i++)
				{
					m_szFiles[i] = new TCHAR[MAX_PATH];
					DragQueryFile((HDROP)medium.hGlobal, i, m_szFiles[i], MAX_PATH * sizeof(TCHAR));
				}

				if ( this->TagHelper.OpenDb() )
				{
					this->TagHelper.SetCurrentFiles(m_szFiles,FileCount);
					this->TagHelper.LoadTags();

					hr = S_OK;
				}
			}

			ReleaseStgMedium(&medium);
		}
	}

	return hr; 
}
