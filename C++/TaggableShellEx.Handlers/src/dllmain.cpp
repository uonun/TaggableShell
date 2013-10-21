// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//
// Copyright (c) Microsoft Corporation. All rights reserved

#include "../include/dllmain.h"

typedef HRESULT (*PFNCREATEINSTANCE)(REFIID riid, void **ppvObject);
struct CLASS_OBJECT_INIT
{
	const CLSID *pClsid;
	PFNCREATEINSTANCE pfnCreate;
};

// add classes supported by this module here
const CLASS_OBJECT_INIT c_rgClassObjectInit[] =
{
	{ &__uuidof(CHandler), CHandler_CreateInstance },
};


long g_cRefModule = 0;

// Handle the the DLL's module
HINSTANCE g_hInst = NULL;
WCHAR g_DllFullName[MAX_PATH];
WCHAR g_DllDirectory[MAX_PATH];
WCHAR g_ProfileDirectory[MAX_PATH];
WCHAR g_UserDb[MAX_PATH];

#ifdef LOG4CPP
WCHAR g_LogDirectory[MAX_PATH];
WCHAR g_LogFullName[MAX_PATH];
#endif
///////////////////////////////////////////////////////////////////////////

// Standard DLL functions
STDAPI_(BOOL) DllMain(HINSTANCE hInstance, DWORD dwReason, void *)
{
	::CoInitialize(NULL);
	locale("chs");

	GetModuleFileName(hInstance,g_DllFullName, sizeof(g_DllFullName) / sizeof(g_DllFullName[0]) - 1);
	std::tr2::sys::wpath p(g_DllFullName);
	wpath dir = p.parent_path();
	wsprintf ( g_DllDirectory, L"%s", dir.string().c_str() );

#ifdef LOG4CPP
	wsprintf ( g_LogDirectory, L"%s/%s\0", g_DllDirectory,FOLDER_LOG );
	wsprintf ( g_LogFullName, L"%s/%s\0",g_LogDirectory,LOG_FILENAME );
	::Replace(g_LogFullName,'/','\\');
	OutputDebugString(L"Log file: ");
	OutputDebugString(g_LogFullName);
	OutputDebugString(L"\n");
#endif

	if (dwReason == DLL_PROCESS_ATTACH)
	{
		wsprintf ( g_ProfileDirectory, L"%s/%s\0", g_DllDirectory,FOLDER_PROFILE );

		std::tr2::sys::wpath p_g_ProfileDirectory(g_ProfileDirectory); 
		auto directory_string_ProfileDirectory = p_g_ProfileDirectory.directory_string(); // change the "/" to "\"
		wsprintf ( g_UserDb, L"%s\\%s\0", directory_string_ProfileDirectory.c_str(),FILE_USERDB );

		if (
			(CreateDirectory(g_ProfileDirectory, NULL) || ERROR_ALREADY_EXISTS == GetLastError())
#ifdef LOG4CPP
			&&(CreateDirectory(g_LogDirectory, NULL) || ERROR_ALREADY_EXISTS == GetLastError())
#endif
			)
		{
			::PrintLog(L"===================================================================================================");
			::PrintLog(L"DllMain, dwReason = %d. (DLL_PROCESS_DETACH = 0, DLL_PROCESS_ATTACH = 1, DLL_THREAD_ATTACH = 2, DLL_THREAD_DETACH = 3)",dwReason);
			::PrintLog(L"OS Version: %s", GetOSVersionStr());
			::PrintLog(L"	g_DllDirectory:			%s",g_DllDirectory);
			::PrintLog(L"	g_DllFullName:			%s",g_DllFullName);
			::PrintLog(L"	g_ProfileDirectory:		%s",g_ProfileDirectory);
			::PrintLog(L"	g_UserDb:				%s",g_UserDb);
#ifdef LOG4CPP
			::PrintLog(L"	g_LogDirectory:			%s",g_LogDirectory);
			::PrintLog(L"	g_LogFullName:			%s",g_LogFullName);
#endif
			g_hInst = hInstance;
			DisableThreadLibraryCalls(hInstance);
		}
		else
		{
			// Failed to create log directory.
			return FALSE;
		}
	}else{
		::PrintLog(L"DllMain Exit.\r\n\r\n\r\n",dwReason);
	}
	return TRUE;
}

STDAPI DllCanUnloadNow()
{
	// Only allow the DLL to be unloaded after all outstanding references have been released
	auto a = (g_cRefModule <= 0) ? S_OK : S_FALSE;
	if(a == S_OK){
		CoUninitialize();
	}
	return a;
}

void DllAddRef()
{
	InterlockedIncrement(&g_cRefModule);
}

void DllRelease()
{
	InterlockedDecrement(&g_cRefModule);
}

class CClassFactory : public IClassFactory
{
public:
	static HRESULT CreateInstance(REFCLSID clsid, const CLASS_OBJECT_INIT *pClassObjectInits, size_t cClassObjectInits, REFIID riid, void **ppv)
	{
		*ppv = NULL;
		HRESULT hr = CLASS_E_CLASSNOTAVAILABLE;
		for (size_t i = 0; i < cClassObjectInits; i++)
		{
			if (clsid == *pClassObjectInits[i].pClsid)
			{
				IClassFactory *pClassFactory = new (std::nothrow) CClassFactory(pClassObjectInits[i].pfnCreate);
				hr = pClassFactory ? S_OK : E_OUTOFMEMORY;
				if (SUCCEEDED(hr))
				{
					hr = pClassFactory->QueryInterface(riid, ppv);
					pClassFactory->Release();
				}
				break; // match found
			}
		}
		return hr;
	}

	CClassFactory(PFNCREATEINSTANCE pfnCreate) : _cRef(1), _pfnCreate(pfnCreate)
	{
		::PrintLog(L"CClassFactory.ctor");
		DllAddRef();
	}

	// IUnknown
	IFACEMETHODIMP QueryInterface(REFIID riid, void ** ppv)
	{
		static const QITAB qit[] =
		{
			QITABENT(CClassFactory, IClassFactory),
			{ 0 }
		};
		return QISearch(this, qit, riid, ppv);
	}

	IFACEMETHODIMP_(ULONG) AddRef()
	{
		return InterlockedIncrement(&_cRef);
	}

	IFACEMETHODIMP_(ULONG) Release()
	{
		long cRef = InterlockedDecrement(&_cRef);
		if (cRef == 0)
		{
			delete this;
		}
		return cRef;
	}

	// IClassFactory
	IFACEMETHODIMP CreateInstance(IUnknown *punkOuter, REFIID riid, void **ppv)
	{
		return punkOuter ? CLASS_E_NOAGGREGATION : _pfnCreate(riid, ppv);
	}

	IFACEMETHODIMP LockServer(BOOL fLock)
	{
		if (fLock)
		{
			DllAddRef();
		}
		else
		{
			DllRelease();
		}
		return S_OK;
	}

private:
	~CClassFactory()
	{
		DllRelease();
	}

	long _cRef;
	PFNCREATEINSTANCE _pfnCreate;
};

STDAPI DllGetClassObject(REFCLSID clsid, REFIID riid, void **ppv)
{
	::PrintLog(L"DllGetClassObject");
	return CClassFactory::CreateInstance(clsid, c_rgClassObjectInit, ARRAYSIZE(c_rgClassObjectInit), riid, ppv);
}

STDAPI DllRegisterServer()
{	
	::PrintLog(L"DllRegisterServer");
	return S_OK;
	//return RegisterHandler();
}

STDAPI DllUnregisterServer()
{
	::PrintLog(L"DllUnregisterServer");
	return S_OK;
	//return UnregisterHandler();
}
