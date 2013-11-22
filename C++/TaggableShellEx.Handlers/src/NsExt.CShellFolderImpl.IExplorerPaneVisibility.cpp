#pragma once
#include "../include/NsExt.CShellFolderImpl.h"

// IExplorerPaneVisibility
HRESULT CShellFolderImpl::GetPaneState(REFEXPLORERPANE ep, EXPLORERPANESTATE *peps)
{
	*peps = EPS_DONTCARE;
	if (IsEqualIID( EP_NavPane, ep) 
		|| IsEqualIID( EP_Commands, ep)
		|| IsEqualIID( EP_Commands_Organize, ep)
		|| IsEqualIID( EP_Commands_View, ep)
		|| IsEqualIID( EP_DetailsPane, ep)
		)
	{
		*peps = EPS_DEFAULT_ON | EPS_FORCE;
	}

	return S_OK;
}
