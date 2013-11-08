// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//
// Copyright (c) Microsoft Corporation. All rights reserved

#include "../include/dllmain.h"
#include "../include/RegisterExtension.h"

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
	{ &__uuidof(CShellFolderImpl), CShellFolderImpl_CreateInstance },
	{ &__uuidof(CShellViewImpl), CShellViewImpl_CreateInstance },
};


long g_cRefModule = 0;

// Handle the the DLL's module
HINSTANCE g_hInst = NULL;
WCHAR g_AppRoot[MAX_PATH] = {0};
WCHAR g_DllFullName[MAX_PATH] = {0};
WCHAR g_DllDirectory[MAX_PATH] = {0};
WCHAR g_ProfileDirectory[MAX_PATH] = {0};
WCHAR g_UserDb[MAX_PATH] = {0};
WCHAR g_LogDirectory[MAX_PATH] = {0};

#ifdef LOG4CPP
WCHAR g_LogFullName[MAX_PATH];
#endif

const BOOL DEFAULT_SETTINGS_SHOW_DESKTOP = FALSE;	//there is a bug that the tags will be listed multi-times in the Folder when showing on desktop
const BOOL DEFAULT_SETTINGS_SHOW_MYCOMPUTER = TRUE;

#ifdef SUPPORT_SETTINGS_IN_REG
#define REG_SETTINGS_NAME						L"TaggableShellSettings"
#define REG_SETTINGS_VALUENAME_SHOW_DESKTOP		L"TaggableShellSettings.ShowOnDesktop"
#define REG_SETTINGS_VALUENAME_SHOW_MYCOMPUTER	L"TaggableShellSettings.ShowInMyComputer"
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
	StrCpy(g_DllDirectory,dir.string().c_str() );

	wpath rootDir = dir.parent_path();
	StrCpy(g_AppRoot,rootDir.string().c_str());

	WCHAR tmp_g_LogDirectory[MAX_PATH] = {0};
	wsprintf ( tmp_g_LogDirectory, L"%s/%s\0", g_AppRoot,FOLDER_LOG );
	std::tr2::sys::wpath p_g_LogDirectory(tmp_g_LogDirectory); 
	StrCpy(g_LogDirectory,p_g_LogDirectory.directory_string().c_str()); // change the "/" to "\"

#ifdef LOG4CPP
	wsprintf ( g_LogFullName, L"%s\\%s\0",g_LogDirectory,LOG_FILENAME );

	OutputDebugString(L"Log file: ");
	OutputDebugString(g_LogFullName);
	OutputDebugString(L"\r\n");
#endif

	if (dwReason == DLL_PROCESS_ATTACH)
	{
		wsprintf ( g_ProfileDirectory, L"%s/%s\0", g_AppRoot,FOLDER_PROFILE );

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
			::PrintLog(L"DllMain, DLL_PROCESS_ATTACH");
			::PrintLog(L"OS Version: %s", GetOSVersionStr());
			::PrintLog(L"	g_AppRoot:				%s",g_AppRoot);
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
			// Failed to create log/profile directory.
			return FALSE;
		}
	}
	else if( dwReason == DLL_THREAD_ATTACH)
	{
		::PrintLog(L"DllMain, DLL_THREAD_ATTACH.\r\n\r\n\r\n");
	}
	else if( dwReason == DLL_THREAD_DETACH)
	{
		::PrintLog(L"DllMain, DLL_THREAD_DETACH.\r\n\r\n\r\n");
	}
	else if( dwReason == DLL_PROCESS_DETACH)
	{
		::PrintLog(L"DllMain, DLL_PROCESS_DETACH.\r\n\r\n\r\n");
	}
	else
	{
		::PrintLog(L"DllMain, dwReason = %d. (DLL_PROCESS_DETACH = 0, DLL_PROCESS_ATTACH = 1, DLL_THREAD_ATTACH = 2, DLL_THREAD_DETACH = 3)",dwReason);
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
	HRESULT hr = S_FALSE;

	// buffer for strings to Register.
	LPCWSTR _regStrBufferTmp = NULL;

	WCHAR c_szProgID[LOADSTRING_BUFFERSIZE] = {0}
	,c_szInfoTip[LOADSTRING_BUFFERSIZE] = {0}
	,c_szIntroText[LOADSTRING_BUFFERSIZE] = {0}
	,c_szLocalizedString[LOADSTRING_BUFFERSIZE] = {0}
	,folderFriendlyName[LOADSTRING_BUFFERSIZE] = {0}
	,handlerFriendlyName[LOADSTRING_BUFFERSIZE] = {0};

	_regStrBufferTmp = ::MyLoadString(IDS_REG_ProgID);
	StrCpy(c_szProgID,_regStrBufferTmp);

	_regStrBufferTmp = ::MyLoadString(IDS_REG_ProgID_InfoTip);
	StrCpy(c_szInfoTip,_regStrBufferTmp);

	_regStrBufferTmp =  ::MyLoadString(IDS_REG_ProgID_IntroText);
	StrCpy(c_szIntroText,_regStrBufferTmp);

	_regStrBufferTmp = ::MyLoadString(IDS_REG_ProgID_LocalizedString);
	StrCpy(c_szLocalizedString,_regStrBufferTmp);

	_regStrBufferTmp = ::MyLoadString(IDS_REG_FOLDER_FriendlyName);
	StrCpy(folderFriendlyName,_regStrBufferTmp);

	_regStrBufferTmp = ::MyLoadString(IDS_REG_HANDLER_FriendlyName);
	StrCpy(handlerFriendlyName,_regStrBufferTmp);

#pragma region register namespace extension
	// register the shell folder with the system
	CRegisterExtension reFolder(__uuidof(CShellFolderImpl), HKEY_LOCAL_MACHINE);

	//[HKEY_LOCAL_MACHINE\SOFTWARE\Classes\CLSID\<clsid>]
	hr = reFolder.RegisterCLSID(folderFriendlyName, L"Apartment",TRUE, c_szInfoTip,c_szIntroText,c_szLocalizedString);
	if (SUCCEEDED(hr))
	{
		//[HKEY_LOCAL_MACHINE\SOFTWARE\Classes\CLSID\<clsid>\DefaultIcon]
		hr = reFolder.RegisterDefaultIcon(IDI_ICON);

		if (SUCCEEDED(hr))
		{
			//[HKEY_LOCAL_MACHINE\SOFTWARE\Classes\CLSID\<clsid>\ShellFolder]
			//the first argument "attributes" see: http://msdn.microsoft.com/en-us/library/bb762589(v=vs.85).aspx
			hr = reFolder.RegisterShellFolder(0xA0005040,TRUE);

			if (SUCCEEDED(hr))
			{
				// register the shell view with the system
				CRegisterExtension reView(__uuidof(CShellViewImpl), HKEY_LOCAL_MACHINE);
				hr = reView.RegisterCLSID(folderFriendlyName, L"Apartment",TRUE, c_szInfoTip,c_szIntroText,c_szLocalizedString);
				if (SUCCEEDED(hr))
				{
					//http://msdn.microsoft.com/en-us/library/cc144096(v=vs.85).aspx
					//[HKEY_LOCAL_MACHINE\Software\Microsoft\Windows\CurrentVersion\Explorer\<parent>\NameSpace\<clsid>]
					if ( DEFAULT_SETTINGS_SHOW_DESKTOP )
					{
						hr = reFolder.RegisterShellFolderNameSpace(folderFriendlyName,L"Desktop");
					}

					if ( DEFAULT_SETTINGS_SHOW_MYCOMPUTER )
					{
						hr = reFolder.RegisterShellFolderNameSpace(folderFriendlyName,L"MyComputer");
					}
				}
			}
		}
	}
#pragma endregion

	if (FAILED(hr))
		return hr;

#pragma region register handler
	// register the context menu handler with the system
	CRegisterExtension reHandler(__uuidof(CHandler), HKEY_LOCAL_MACHINE);

	//[HKEY_LOCAL_MACHINE\SOFTWARE\Classes\CLSID\<clsid>]
	hr = reHandler.RegisterCLSID(handlerFriendlyName, L"Apartment",TRUE, c_szInfoTip,c_szIntroText,c_szLocalizedString);
	if (SUCCEEDED(hr))
	{
		hr = reHandler.RegisterContextMenu(L"*");
		if (SUCCEEDED(hr))
		{
			hr = reHandler.RegisterContextMenu(L"Directory");
			if (SUCCEEDED(hr))
			{
				hr = reHandler.RegisterContextMenu(L"Folder");
			}
		}

		hr = reHandler.RegisterPropertyPage(L"*");
		if (SUCCEEDED(hr))
		{
			hr = reHandler.RegisterPropertyPage(L"Directory");
			if (SUCCEEDED(hr))
			{
				hr = reHandler.RegisterPropertyPage(L"Folder");
			}
		}
	}
#pragma endregion

	if (FAILED(hr))
		return hr;

#ifdef SUPPORT_SETTINGS_IN_REG
#pragma region register settings
	WCHAR settingsLocalizedName[LOADSTRING_BUFFERSIZE] = {0}
	,szShowOnDesktop[LOADSTRING_BUFFERSIZE] = {0}
	,szShowInMyComputer[LOADSTRING_BUFFERSIZE] = {0};

	_regStrBufferTmp = ::MyLoadString(IDS_ProductName);
	StrCpy(settingsLocalizedName,_regStrBufferTmp);

	_regStrBufferTmp = ::MyLoadString(IDS_REG_SETTINGS_SHOW_DESKTOP);
	StrCpy(szShowOnDesktop,_regStrBufferTmp);

	_regStrBufferTmp = ::MyLoadString(IDS_REG_SETTINGS_SHOW_MYCOMPUTER);
	StrCpy(szShowInMyComputer,_regStrBufferTmp);

	CRegisterExtension reSettings(__uuidof(CHandler), HKEY_LOCAL_MACHINE);
	hr = reSettings.RegisterSettings(
		REG_SETTINGS_NAME,settingsLocalizedName,
		szShowOnDesktop,REG_SETTINGS_VALUENAME_SHOW_DESKTOP,DEFAULT_SETTINGS_SHOW_DESKTOP,
		szShowInMyComputer,REG_SETTINGS_VALUENAME_SHOW_MYCOMPUTER,DEFAULT_SETTINGS_SHOW_MYCOMPUTER
		);
#pragma endregion
#endif

	return hr;
}

STDAPI DllUnregisterServer()
{
	HRESULT hr = S_FALSE;

#pragma region unregister namespace extension
	// Remove the COM object registration.
	CRegisterExtension reFolder(__uuidof(CShellFolderImpl), HKEY_LOCAL_MACHINE);
	hr = reFolder.UnRegisterObject();
	if (SUCCEEDED(hr))
	{
		// Remove the COM object registration.
		CRegisterExtension reView(__uuidof(CShellViewImpl), HKEY_LOCAL_MACHINE);
		hr = reView.UnRegisterObject();
		if (SUCCEEDED(hr))
		{
			hr = reFolder.UnRegisterShellFolderNameSpace(L"Desktop");
			hr = reFolder.UnRegisterShellFolderNameSpace(L"MyComputer");
		}
	}
#pragma endregion

#pragma region unregister handler
	CRegisterExtension reHandler(__uuidof(CHandler), HKEY_LOCAL_MACHINE);
	hr = reHandler.UnRegisterObject();
	if (SUCCEEDED(hr))
	{
		reHandler.UnRegisterContextMenu(L"*");
		reHandler.UnRegisterContextMenu(L"Directory");
		reHandler.UnRegisterContextMenu(L"Folder");

		reHandler.UnRegisterPropertyPage(L"*");
		reHandler.UnRegisterPropertyPage(L"Directory");
		reHandler.UnRegisterPropertyPage(L"Folder");
	}
#pragma endregion

#ifdef SUPPORT_SETTINGS_IN_REG
	CRegisterExtension reSettings(__uuidof(CHandler), HKEY_LOCAL_MACHINE);
	hr = reSettings.UnRegisterSettings(REG_SETTINGS_NAME,REG_SETTINGS_VALUENAME_SHOW_DESKTOP,REG_SETTINGS_VALUENAME_SHOW_MYCOMPUTER);
#endif

	return hr;
}
