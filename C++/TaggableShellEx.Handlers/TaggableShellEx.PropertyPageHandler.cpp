#pragma once
#include <windows.h>
#include <shlwapi.h>
#include <shlobj.h>
#include <PrSht.h>
#include "TaggableShellEx.PropertyPageHandler.h"
#include "dllmain.h"
#include "resource.h"

PropertyPageHandler::PropertyPageHandler() : _cRef(1),_pStream(NULL)
{
	DllAddRef();
#ifdef LOG4CPP
	Utils::PrintLog(L"PropertyPageHandler.ctor");
#endif

	//m_pIDFolder = (LPITEMIDLIST)malloc(sizeof(LPITEMIDLIST));
	m_pIDFolder = NULL;
	*m_szFile = NULL;
	m_pDataObj = NULL;
	m_hRegKey = NULL;
	g_DllRefCount = 0;
}

PropertyPageHandler::~PropertyPageHandler(void){   
#ifdef LOG4CPP
	Utils::PrintLog(L"PropertyPageHandler.~ctor");
#endif
	DllRelease();
}

HRESULT PropertyPageHandler_CreateInstance(REFIID riid, void **ppv)
{
#ifdef LOG4CPP
	Utils::PrintLog(L"PropertyPageHandler.PropertyPageHandler_CreateInstance");
#endif
	PropertyPageHandler *pNew = new(std::nothrow) PropertyPageHandler;
	HRESULT hr = pNew ? S_OK : E_OUTOFMEMORY;
	if (pNew)
	{
		hr = pNew->QueryInterface(riid, ppv);
		pNew->Release();
	}
	return hr;
}

// IUnknown
IFACEMETHODIMP PropertyPageHandler::QueryInterface(REFIID riid, void ** ppv)
{
	static const QITAB qit[] =
	{
		QITABENT(PropertyPageHandler, IShellExtInit),
		QITABENT(PropertyPageHandler, IShellPropSheetExt),
		{0},
	};
	return QISearch(this, qit, riid, ppv);
}

IFACEMETHODIMP_(ULONG) PropertyPageHandler::AddRef()
{
	return InterlockedIncrement(&_cRef);
}

IFACEMETHODIMP_(ULONG) PropertyPageHandler::Release()
{
	long cRef = InterlockedDecrement(&_cRef);
	if (cRef == 0)
	{
		delete this;
	}
	return cRef;
}

HRESULT PropertyPageHandler::Initialize(LPCITEMIDLIST pIDFolder, 
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
	}
	return S_OK; 
}

INT_PTR CALLBACK PageDlgProc(     
	_In_  HWND hwnd,     
	_In_  UINT uMsg,     
	_In_  WPARAM wParam,     
	_In_  LPARAM lParam     
	)     
{
	BOOL bRet = FALSE;

	switch ( uMsg )
	{
	case WM_INITDIALOG:
		//bRet = OnInitDialog ( hwnd, lParam );
		break;

	case WM_NOTIFY:
		{
			NMHDR* phdr = (NMHDR*) lParam;

			switch ( phdr->code )
			{
			case PSN_APPLY:
				//bRet = OnApply ( hwnd, (PSHNOTIFY*) phdr );
				break;

			case DTN_DATETIMECHANGE:
				// If the user changes any of the DTP controls, enable
				// the Apply button.
				SendMessage ( GetParent(hwnd), PSM_CHANGED, (WPARAM) hwnd, 0 );
				break;
			}
		}
		break;
	}

	return bRet;
}

UINT CALLBACK PageCallbackProc(
	HWND hwnd,
	_In_     UINT uMsg,
	_Inout_  LPPROPSHEETPAGE ppsp
	)
{
	return 1;   // use nonzero let the page be created
}

STDMETHODIMP PropertyPageHandler::AddPages(LPFNADDPROPSHEETPAGE lpfnAddPage, LPARAM lParam)
{
#ifdef LOG4CPP
	Utils::PrintLog(L"PropertyPageHandler.AddPages");
#endif

	PROPSHEETPAGE  psp;
	HPROPSHEETPAGE hPage;

	// Set up the PROPSHEETPAGE struct.
	ZeroMemory ( &psp, sizeof(PROPSHEETPAGE) );

	psp.dwSize      = sizeof(PROPSHEETPAGE);
	psp.dwFlags     = PSP_USEREFPARENT | PSP_USETITLE | PSP_DEFAULT | PSP_USECALLBACK;
	psp.hInstance   = ::g_hInst;
	psp.pszTemplate = MAKEINTRESOURCE(IDD_PAGEDLG);
	psp.pszIcon     = 0;
	psp.pszTitle    = L"pszTitle";
	psp.pfnDlgProc  = PageDlgProc;
	psp.lParam      = (LPARAM) this;
	psp.pfnCallback = PageCallbackProc;
	psp.pcRefParent = (UINT*) &g_DllRefCount;

	// Create the page & get a handle.
	hPage = CreatePropertySheetPage ( &psp );

	if ( NULL != hPage )
	{
		// Call the shell's callback function, so it adds the page to
		// the property sheet.
		if ( lpfnAddPage ( hPage, lParam )){
			this->AddRef();
		}
		else
		{
			DestroyPropertySheetPage ( hPage );
		}
	}else{
		return E_OUTOFMEMORY;
	}

	return S_OK;
}

