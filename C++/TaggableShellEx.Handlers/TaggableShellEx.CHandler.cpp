#pragma once
#include "dllmain.h"
#include "resource.h"
#include "TaggableShellEx.CHandler.h"
#include "Taghelper.h"

CHandler::CHandler() : 
	_cRef(1) // IUnknown
	,m_pIDFolder(NULL),m_pDataObj(NULL),m_hRegKey(NULL)	// IShellExtInit
	,g_DllRefCount(0) // IShellPropSheetExt
{
	// IShellExtInit
	memset(m_szFile,0,sizeof(m_szFile)/sizeof(m_szFile[0]));

	DllAddRef();
	::PrintLog(L"CHandler.ctor");
}

CHandler::~CHandler(void)
{   
	::PrintLog(L"CHandler.~ctor");
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
        UINT        uCount;

        if(SUCCEEDED(m_pDataObj->GetData(&fe, &medium)))
        {
            // Get the count of files dropped.
            uCount = DragQueryFile((HDROP)medium.hGlobal, (UINT)-1, NULL, 0);

            // Get the first file name from the CF_HDROP.
            if(uCount)
                DragQueryFile((HDROP)medium.hGlobal, 0, m_szFile, 
                              sizeof(m_szFile)/sizeof(TCHAR));

            ReleaseStgMedium(&medium);
        }
	}

	// load tags
	_tagHelper.LoadTags();

	return S_OK; 
}
