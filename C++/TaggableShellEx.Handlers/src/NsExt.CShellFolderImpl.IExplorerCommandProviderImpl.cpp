#pragma once
#include "..\include\NsExt.CShellFolderImpl.h"
#include "..\include\NsExt.CEnumExplorerCommandImpl.h"


// IExplorerCommandProvider
HRESULT CShellFolderImpl::GetCommands(IUnknown *punkSite,REFIID riid,void **ppv)
{
	HRESULT hr = E_NOTIMPL;
	CEnumExplorerCommandImpl *cmds = new CEnumExplorerCommandImpl(this);
	hr = cmds->QueryInterface(riid,ppv);
	cmds->Release();
	return hr;
}

HRESULT CShellFolderImpl::GetCommand(REFGUID rguidCommandId,REFIID riid,void **ppv)
{
	HRESULT hr = E_NOTIMPL;
	return hr;
}
