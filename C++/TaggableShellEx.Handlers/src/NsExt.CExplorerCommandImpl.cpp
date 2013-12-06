#pragma once
#include "..\include\NsExt.CExplorerCommandImpl.h"
#include "..\include\NsExt.CShellFolderImpl.h"

CExplorerCommandImpl::CExplorerCommandImpl(void):
	_cRef(1)
	,_iconID(0)
	,m_pFolder(NULL)
{

}


CExplorerCommandImpl::~CExplorerCommandImpl(void)
{

}

// IUnknown
IFACEMETHODIMP CExplorerCommandImpl::QueryInterface(REFIID riid, void ** ppv)
{
	static const QITAB qit[] =
	{
		QITABENT(CExplorerCommandImpl, IExplorerCommand),
		{0},
	};
	return QISearch(this, qit, riid, ppv);
}

IFACEMETHODIMP_(ULONG) CExplorerCommandImpl::AddRef()
{
	return InterlockedIncrement(&_cRef);
}

IFACEMETHODIMP_(ULONG) CExplorerCommandImpl::Release()
{
	long cRef = InterlockedDecrement(&_cRef);
	if (cRef == 0)
	{
		delete this;
	}
	return cRef;
}

HRESULT CExplorerCommandImpl::Init(CShellFolderImpl* pFolder, const MENUITEMINFO& mii, const UINT & iconID)
{
	::PrintLog(L"CExplorerCommandImpl::Init\n");
	m_pFolder = pFolder;
	// Copy menu-information and switch string buffer to one we own
	m_mii = mii;
	wcscpy_s(m_wszTitle,ARRAYSIZE(m_wszTitle), mii.dwTypeData);
	m_mii.dwTypeData = m_wszTitle;
	_iconID = iconID;
	return S_OK;
}

// IExplorerCommand
STDMETHODIMP CExplorerCommandImpl::GetTitle(IShellItemArray* psiItemArray, LPWSTR* ppszName)
{
	return ::SHStrDupW(m_mii.dwTypeData, ppszName);
}

STDMETHODIMP CExplorerCommandImpl::GetIcon(IShellItemArray* psiItemArray, LPWSTR* ppszIcon)
{
	WCHAR wszIconPath[MAX_PATH + 20] = { 0 };
	::wnsprintf(wszIconPath, ARRAYSIZE(wszIconPath) - 1, L"%s,-%d", g_DllFullName,_iconID);
	return ::SHStrDupW(wszIconPath, ppszIcon);
}

STDMETHODIMP CExplorerCommandImpl::GetToolTip(IShellItemArray* psiItemArray, LPWSTR* ppszInfotip)
{
	LPCWSTR pSep = L"GetToolTip";
	if( pSep == NULL ) return E_FAIL;
	return ::SHStrDupW(pSep, ppszInfotip);
}

STDMETHODIMP CExplorerCommandImpl::GetCanonicalName(GUID* pguidCommandName)
{
	*pguidCommandName = GetCanonicalName(m_mii.wID);
	return S_OK;
}

STDMETHODIMP CExplorerCommandImpl::GetState(IShellItemArray* psiItemArray, BOOL fOkToBeSlow, EXPCMDSTATE* pCmdState)
{
	*pCmdState = 0;
	if( (m_mii.fState & MFS_CHECKED) != 0 ) *pCmdState |= ECS_CHECKED;
	if( (m_mii.fState & MFS_DISABLED) != 0 ) *pCmdState |= ECS_DISABLED;
	return S_OK;
}

STDMETHODIMP CExplorerCommandImpl::Invoke(IShellItemArray* psiItemArray, IBindCtx* pbc)
{
	::PrintLog(L"CExplorerCommandImpl::Invoke\n");
	// Execute menu command.
	// If nobody cared for the command, we will try it directly on the folder.
	// This allows commands to work on a specific folder-level even when no items are selected.
	HRESULT Hr = E_NOTIMPL;
	Hr = m_pFolder->ExecuteMenuCommand(this,psiItemArray,pbc);
	//if( Hr == S_FALSE ) Hr = m_spProvider->m_spFolder->m_spFolderItem->ExecuteMenuCommand(Cmd);
	return Hr;
}

STDMETHODIMP CExplorerCommandImpl::GetFlags(EXPCMDFLAGS* pFlags)
{
	*pFlags = 0;
	if( m_mii.hSubMenu != NULL ) *pFlags |= ECF_HASSUBCOMMANDS;
	if( m_mii.fType & MFT_SEPARATOR ) *pFlags |= ECF_ISSEPARATOR;
	return S_OK;
}

STDMETHODIMP CExplorerCommandImpl::EnumSubCommands(IEnumExplorerCommand** ppEnum)
{
	HRESULT Hr = E_NOTIMPL;
	return Hr;
}

// Static members
GUID CExplorerCommandImpl::GetCanonicalName(UINT wID)
{
	// Derive menu-item GUIDs from the ShellFolder CLSID; violating
	// a few GUID uniqueness rules here perhaps.
	GUID guid = __uuidof(CShellFolderImpl);
	guid.Data2 = 0xEFFE;
	guid.Data1 = (USHORT) wID;
	return guid;
}

UINT CExplorerCommandImpl::GetMenuID(GUID guid)
{
	return guid.Data2 == 0xEFFE ? guid.Data1 : 0;
}