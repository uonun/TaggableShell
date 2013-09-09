// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "stdafx.h"
#include <comdef.h>

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
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