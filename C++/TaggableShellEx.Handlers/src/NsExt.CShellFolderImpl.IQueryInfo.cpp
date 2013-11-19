#pragma once
#include "../include/NsExt.CShellFolderImpl.h"

// IQueryInfo
HRESULT CShellFolderImpl::GetInfoTip(DWORD dwFlags, PWSTR *ppwszTip)
{
	ppwszTip = (PWSTR*)CoTaskMemAlloc(12);
	memcpy(ppwszTip,L"GetInfoTip",7);
	return S_OK;
}

// IExplorerPaneVisibility
HRESULT CShellFolderImpl::GetPaneState(REFEXPLORERPANE ep, EXPLORERPANESTATE *peps)
{
	*peps = EPS_DONTCARE;
	if ( IsEqualIID( EP_PreviewPane, ep) 
		|| IsEqualIID( EP_Commands_Organize, ep) 
		|| IsEqualIID( EP_AdvQueryPane, ep)
		)
	{
		*peps = EPS_DEFAULT_OFF | EPS_FORCE;
	}

	return S_OK;
}
