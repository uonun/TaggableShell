#pragma once
#include "../include/NsExt.CShellFolderImpl.h"

// IQueryInfo
HRESULT CShellFolderImpl::GetInfoTip(DWORD dwFlags, PWSTR *ppwszTip)
{
	ppwszTip = (PWSTR*)CoTaskMemAlloc(12);
	memcpy(ppwszTip,L"SDFSDF",7);
	return S_OK;
}
