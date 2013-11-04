#pragma once
#include "../include/NsExt.CShellFolderImpl.h"

// IPersistFolder
HRESULT CShellFolderImpl::Initialize(LPCITEMIDLIST pIDFolder)
{
	::PrintLog(L"CShellFolderImpl::Initialize");

	// If Initialize has already been called, release the old PIDL
	if(m_pIDFolder!=NULL){
		ILFree(m_pIDFolder);
		m_pIDFolder = NULL;
	}

	//Store the new PIDL.
	if(pIDFolder)
	{
		m_pIDFolder = ILClone(pIDFolder);

		//_initialized = TRUE;

		SHGetPathFromIDList(m_pIDFolder,_FolderPath);
		::PrintLog(L"CShellFolderImpl::Initialize: Get in path: %s",_FolderPath);

		/*
		SIGDN_NORMALDISPLAY				MyFolder
		SIGDN_PARENTRELATIVEPARSING		MyFolder.{8F08EB60-3F01-1982-0805-C2DEBAE9C1D6}
		SIGDN_DESKTOPABSOLUTEPARSING	E:\Works\UDNZ\udnz.com.ShellEx.TaggableShell\C++\_Debug\x64\MyFolder.{8F08EB60-3F01-1982-0805-C2DEBAE9C1D6}
		SIGDN_PARENTRELATIVEEDITING		MyFolder
		SIGDN_DESKTOPABSOLUTEEDITING	E:\Works\UDNZ\udnz.com.ShellEx.TaggableShell\C++\_Debug\x64\MyFolder
		SIGDN_FILESYSPATH				E:\Works\UDNZ\udnz.com.ShellEx.TaggableShell\C++\_Debug\x64\MyFolder.{8F08EB60-3F01-1982-0805-C2DEBAE9C1D6}
		SIGDN_URL						file:///E:/Works/UDNZ/udnz.com.ShellEx.TaggableShell/C++/_Debug/x64/MyFolder.%7B8F08EB60-3F01-1982-0805-C2DEBAE9C1D6%7D
		SIGDN_PARENTRELATIVEFORADDRESSBAR			MyFolder
		SIGDN_PARENTRELATIVE			MyFolder
		SIGDN_PARENTRELATIVEFORUI		MyFolder

		LPWSTR _path;
		HRESULT hr;
		IShellItem* si;
		hr = SHCreateShellItem(NULL,NULL,m_pIDFolder,&si);
		if ( SUCCEEDED(hr) )
		{
		si->GetDisplayName(SIGDN_NORMALDISPLAY,&_path);
		::PrintLog(L"Get in path: SIGDN_NORMALDISPLAY\t\t\t %s",_path);
		CoTaskMemFree(_path);

		si->GetDisplayName(SIGDN_PARENTRELATIVEPARSING,&_path);
		::PrintLog(L"Get in path: SIGDN_PARENTRELATIVEPARSING\t\t\t %s",_path);
		CoTaskMemFree(_path);

		si->GetDisplayName(SIGDN_DESKTOPABSOLUTEPARSING,&_path);
		::PrintLog(L"Get in path: SIGDN_DESKTOPABSOLUTEPARSING\t\t\t %s",_path);
		CoTaskMemFree(_path);

		si->GetDisplayName(SIGDN_PARENTRELATIVEEDITING,&_path);
		::PrintLog(L"Get in path: SIGDN_PARENTRELATIVEEDITING\t\t\t %s",_path);
		CoTaskMemFree(_path);

		si->GetDisplayName(SIGDN_DESKTOPABSOLUTEEDITING,&_path);
		::PrintLog(L"Get in path: SIGDN_DESKTOPABSOLUTEEDITING\t\t\t %s",_path);
		CoTaskMemFree(_path);

		si->GetDisplayName(SIGDN_FILESYSPATH,&_path);
		::PrintLog(L"Get in path: SIGDN_FILESYSPATH\t\t\t %s",_path);
		CoTaskMemFree(_path);

		si->GetDisplayName(SIGDN_URL,&_path);
		::PrintLog(L"Get in path: SIGDN_URL\t\t\t %s",_path);
		CoTaskMemFree(_path);

		si->GetDisplayName(SIGDN_PARENTRELATIVEFORADDRESSBAR,&_path);
		::PrintLog(L"Get in path: SIGDN_PARENTRELATIVEFORADDRESSBAR\t\t\t %s",_path);
		CoTaskMemFree(_path);

		si->GetDisplayName(SIGDN_PARENTRELATIVE,&_path);
		::PrintLog(L"Get in path: SIGDN_PARENTRELATIVE\t\t\t %s",_path);
		CoTaskMemFree(_path);

		si->GetDisplayName(SIGDN_PARENTRELATIVEFORUI,&_path);
		::PrintLog(L"Get in path: SIGDN_PARENTRELATIVEFORUI\t\t\t %s",_path);
		CoTaskMemFree(_path);
		}
		si->Release();
		*/
	}

	return S_OK;
}

// IPersist which is the base of IPersistFolder
HRESULT CShellFolderImpl::GetClassID(
	CLSID *pClassID
	)
{
	if ( NULL == pClassID )
		return E_POINTER;

	pClassID = (CLSID *)&__uuidof(CShellFolderImpl);

	return S_OK;
}
