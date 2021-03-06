#pragma once
#include "../include/dllmain.h"
#include "../include/Taghelper.h"
#include "../include/CHandler.h"


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
				//MessageBox(hwnd,L"Apply",L"Caption",MB_OK);
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

// http://msdn.microsoft.com/en-us/library/windows/desktop/bb760813(v=vs.85).aspx
UINT CALLBACK PageCallbackProc(
	HWND hwnd,
	_In_     UINT uMsg,
	_Inout_  LPPROPSHEETPAGE ppsp
	)
{
	switch ( uMsg )
	{
	case PSPCB_RELEASE:
		CHandler* tmp = (CHandler*)ppsp->lParam;
		tmp->Release();
		//MessageBox(hwnd,L"PSPCB_RELEASE",L"PSPCB_RELEASE",MB_OK);		
		break;
	}

	return 1;   // use nonzero let the page be created
}

// add property page on an item.
// FILE: right click on a file which could be added to TAG.
// FOLDER: right click on the FOLDER or something else which should not to be added to TAG, such as the FOLDER itself in MyComputer.
STDMETHODIMP CHandler::AddPages(LPFNADDPROPSHEETPAGE lpfnAddPage, LPARAM lParam)
{
	::PrintLog(L"PropertyPageHandler.AddPages");

	PROPSHEETPAGE  psp;
	HPROPSHEETPAGE hPage;

	// Set up the PROPSHEETPAGE struct.
	ZeroMemory ( &psp, sizeof(PROPSHEETPAGE) );

	psp.dwSize      = sizeof(PROPSHEETPAGE);
	psp.dwFlags     = PSP_USEREFPARENT | PSP_USETITLE | PSP_DEFAULT | PSP_USECALLBACK;
	psp.hInstance   = ::g_hInst;
	psp.pszTemplate = MAKEINTRESOURCE(_contextMenuSupposed ? IDD_PROPERTYPAGE_FILE : IDD_SETTINGS);
	psp.pszIcon     = 0;
	psp.pszTitle    = MAKEINTRESOURCE(IDS_DLG_PROPERTYPAGE_CAPTION);
	psp.pfnDlgProc  = PageDlgProc;
	psp.lParam      = (LPARAM) this;
	psp.pfnCallback = PageCallbackProc;
	psp.pcRefParent = (UINT*)&_cRef;

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
