#pragma once
#include "../include/NsExt.CShellFolderImpl.h"

//#define USEICOFORTAG

// IExtractIcon
HRESULT CShellFolderImpl::Extract(PCTSTR pszFile,UINT nIconIndex,HICON *phiconLarge,HICON *phiconSmall,UINT nIconSize)
{
	int sizeBig = 128,sizeSmall = 32;

#ifdef USEICOFORTAG

	*phiconLarge = (HICON)LoadImage(0, pszFile, IMAGE_ICON, sizeBig, sizeBig, LR_LOADFROMFILE);
	if ( NULL == *phiconLarge )
		return S_FALSE;

	*phiconSmall = (HICON)LoadImage(0, pszFile, IMAGE_ICON, sizeSmall, sizeSmall, LR_LOADFROMFILE);
	if ( NULL == *phiconSmall )
		return S_FALSE;

#else
	*phiconLarge = LoadIcon(g_hInst,MAKEINTRESOURCE(IDI_ICON));
	*phiconSmall = *phiconLarge;
#endif 

	nIconSize = MAKELPARAM(sizeBig,sizeSmall);
	return S_OK;
}

HRESULT CShellFolderImpl::GetIconLocation(UINT uFlags,PTSTR pszIconFile,UINT cchMax,int *piIndex,UINT *pwFlags)
{

#ifdef USEICOFORTAG
	wchar_t path[MAX_PATH] = L"E:\\Works\\UDNZ\\udnz.com.ShellEx.TaggableShell\\C++\\_Reference\\tag.ico";
	StrCpy(pszIconFile,path);
	*piIndex  = 1;
#else
	StrCpy(pszIconFile,g_DllFullName);
	cchMax = sizeof(g_DllFullName);
	*piIndex  = IDI_ICON;
#endif 

	*pwFlags = GIL_NOTFILENAME | GIL_PERCLASS;
	if ( uFlags & GIL_ASYNC )
		return E_PENDING;
	return S_OK;
}

