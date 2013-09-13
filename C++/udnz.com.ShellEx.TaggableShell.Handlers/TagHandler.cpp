#pragma once
#include <windows.h>
#include <shlwapi.h>
#include <shlobj.h>
#include <PrSht.h>
#include "TagHandler.h"
#include "dllmain.h"
#include "RegisterExtension.h"
#include "resource.h"

TagHandler::TagHandler() : _cRef(1)
{
	DllAddRef();
}

TagHandler::~TagHandler(void){   
	DllRelease();
}

HRESULT TagHandler_CreateInstance(REFIID riid, void **ppv)
{
	HRESULT hr = E_OUTOFMEMORY;
	TagHandler *pirm = new (std::nothrow) TagHandler();
	if (pirm)
	{
		hr = pirm->QueryInterface(riid, ppv);
		pirm->Release();
	}
	return hr;
}

// IUnknown
IFACEMETHODIMP TagHandler::QueryInterface(REFIID riid, void ** ppv)
{
	static const QITAB qit[] =
	{
		QITABENT(TagHandler, IInitializeWithStream),
		QITABENT(TagHandler, IShellPropSheetExt),
		{0,0 },
	};
	return QISearch(this, qit, riid, ppv);
}

IFACEMETHODIMP_(ULONG) TagHandler::AddRef()
{
	return InterlockedIncrement(&_cRef);
}

IFACEMETHODIMP_(ULONG) TagHandler::Release()
{
	long cRef = InterlockedDecrement(&_cRef);
	if (cRef == 0)
	{
		delete this;
	}
	return cRef;
}

HRESULT TagHandler::Initialize(IStream *pStream, DWORD grfMode)
{
	HRESULT hr = E_UNEXPECTED;
	if (!_pStream)
	{
		// save a reference to the stream as well as the grfMode
		hr = pStream->QueryInterface(&_pStream);
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

STDMETHODIMP TagHandler::AddPages(LPFNADDPROPSHEETPAGE lpfnAddPage, LPARAM lParam)
{
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


/*


/////////////////////////////

const WCHAR c_szZplFileExtension[] = L"._a";
const WCHAR c_szZplProgID[] = L"CLSProgram.1";

HRESULT RegisterHandler()
{
// register the property handler COM object, and set the options it uses
const WCHAR c_szPropertyHandlerDescription[] = L"Taggable Handler";
CRegisterExtension re(__uuidof(TagHandler), HKEY_LOCAL_MACHINE);
HRESULT hr = re.RegisterInProcServer(c_szPropertyHandlerDescription, L"Both");
if (SUCCEEDED(hr))
{
hr = re.RegisterInProcServerAttribute(L"ManualSafeSave", TRUE);
}

// Property Handler registrations use a different mechanism than the rest of the filetype association system, and do not use ProgIDs
if (SUCCEEDED(hr))
{
hr = re.RegisterPropertyHandler(c_szZplFileExtension);
}
return hr;
}
HRESULT UnregisterHandler()
{
// Unregister the property handler COM object.
CRegisterExtension re(__uuidof(TagHandler), HKEY_LOCAL_MACHINE);
HRESULT hr = re.UnRegisterObject();
if (SUCCEEDED(hr))
{
// Unregister the property handler the file extensions.
hr = re.UnRegisterPropertyHandler(c_szZplFileExtension);
}

if (SUCCEEDED(hr))
{
// Remove the whole ProgIDs since we own all of those settings.
// Don't try to remove the file extension association since some other application may have overridden it with their own ProgID in the meantime.
// Leaving the association to a non-existing ProgID is handled gracefully by the Shell.
// NOTE: If the file extension is unambiguously owned by this application, the association to the ProgID could be safely removed as well,
//       along with any other association data stored on the file extension itself.
hr = re.UnRegisterProgID(c_szZplProgID, c_szZplFileExtension);
}
return hr;
}
*/
