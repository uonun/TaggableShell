#pragma once
#include "dllmain.h"
#include "Taghelper.h"
#include "TaggableShellEx.CHandler.h"


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


STDMETHODIMP CHandler::AddPages(LPFNADDPROPSHEETPAGE lpfnAddPage, LPARAM lParam)
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
	psp.pszTitle    = MAKEINTRESOURCE(IDS_PROPERTYPAGE_CAPTION);
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
