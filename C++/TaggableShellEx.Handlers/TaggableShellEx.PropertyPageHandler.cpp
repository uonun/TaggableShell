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
		QITABENT(PropertyPageHandler, IInitializeWithStream),
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

HRESULT PropertyPageHandler::Initialize(IStream *pStream, DWORD grfMode)
{
#ifdef LOG4CPP
	Utils::PrintLog(L"PropertyPageHandler.Initialize");
#endif
	HRESULT hr = E_UNEXPECTED;
	if (!_pStream)
	{
		// save a reference to the stream as well as the grfMode
		hr = pStream->QueryInterface(IID_PPV_ARGS(&_pStream));
		if (SUCCEEDED(hr))
		{
			_grfMode = grfMode;
		}
	}
	return hr;
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

	psp.dwSize        = sizeof(psp);
	psp.dwFlags       = PSP_USEREFPARENT | PSP_USETITLE;// | PSP_USECALLBACK;
	psp.hInstance     = ::g_hInst;
	psp.pszTemplate   = MAKEINTRESOURCE(IDD_PAGEDLG);
	psp.hIcon         = 0;
	psp.pszTitle      = TEXT("Extension Page");
	psp.pfnDlgProc    = (DLGPROC)PageDlgProc;
	psp.pcRefParent   = &g_DllRefCount;
	psp.pfnCallback   = PageCallbackProc;
	psp.lParam        = (LPARAM)this;

	hPage = CreatePropertySheetPage(&psp);

	if(hPage) 
	{
		if(lpfnAddPage(hPage, lParam))
		{
			this->AddRef();
			return S_OK;
		}
		else
		{
			DestroyPropertySheetPage(hPage);
		}
	}
	else
	{
		return E_OUTOFMEMORY;
	}
	return E_FAIL;
}

