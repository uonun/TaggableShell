#include "stdafx.h"
#include <comdef.h>
#include <Prsht.h>
#include "udnz.com.ShellEx.TaggableShell.Core.h"

UDNZSHELLAPI BOOL WINAPI DllMain(
  _In_  HINSTANCE hinstDLL,
  _In_  DWORD fdwReason,
  _In_  LPVOID lpvReserved
)
{
	switch (fdwReason)
	{
	case DLL_PROCESS_ATTACH:
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}

HRESULT __stdcall DllGetClassObject(
  _In_   REFCLSID rclsid,
  _In_   REFIID riid,
  _Out_  LPVOID *ppv) 
{ 
    HRESULT hr = E_OUTOFMEMORY; 
    *ppv = NULL; 
 
	// Create an IKnownFolderManager instance 
	IClassFactory* pClassFactory = NULL; 
	hr = CoCreateInstance(rclsid, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pClassFactory)); 
    if (pClassFactory != NULL)   { 
        hr = pClassFactory->QueryInterface(riid, ppv); 
        pClassFactory->Release(); 
    } 
    return hr;
}

HRESULT __stdcall DllCanUnloadNow(void)
{
	return S_OK;
}
/*
STDMETHODIMP AddPages(LPFNADDPROPSHEETPAGE lpfnAddPage, LPARAM lParam)
{
    PROPSHEETPAGE  psp;
    HPROPSHEETPAGE hPage;

    psp.dwSize        = sizeof(psp);
    psp.dwFlags       = PSP_USEREFPARENT | PSP_USETITLE | PSP_USECALLBACK;
    psp.hInstance     = g_hInst;
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
            //this->AddRef();
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
}*/