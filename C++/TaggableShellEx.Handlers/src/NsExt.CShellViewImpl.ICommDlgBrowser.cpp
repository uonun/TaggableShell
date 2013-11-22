#pragma once
#include "..\include\NsExt.CShellViewImpl.h"


// ICommDlgBrowser
STDMETHODIMP CShellViewImpl::OnDefaultCommand(IShellView * psv)
{
	::PrintLog(L"CShellViewImpl::OnDefaultCommand");

	HRESULT hr;

	IFolderView2 *pfv2;
	hr = _peb->GetCurrentView(IID_PPV_ARGS(&pfv2));
	if (SUCCEEDED(hr))
	{
		IShellItem *psi;
		hr = GetItemFromView(pfv2, -1, IID_PPV_ARGS(&psi));
		if (SUCCEEDED(hr))
		{
			PIDLIST_ABSOLUTE pidl;
			hr = SHGetIDListFromObject(psi, &pidl);
			if (SUCCEEDED(hr))
			{
				LPITEMIDLIST last;
				last = m_PidlMgr.GetLastItem(pidl);
				if( hr == S_OK )
				{
					auto data = m_PidlMgr.GetData(last);
					if ( data != NULL && (data->Type == MYSHITEMTYPE_TAG || data->Type == MYSHITEMTYPE_FILE ) )
					{
						::PrintLog(L"CShellViewImpl::OnDefaultCommand: %s",data->wszDisplayName);
						hr = m_spShellBrowser->BrowseObject(last, SBSP_DEFBROWSER | SBSP_RELATIVE);
						m_spShellBrowser->OnViewWindowActive(psv);
					}
					else
					{
						SHELLEXECUTEINFO ei = { sizeof(ei) };
						ei.fMask        = SEE_MASK_INVOKEIDLIST;
						ei.hwnd         = m_hwndParent;
						ei.nShow        = SW_NORMAL;
						ei.lpIDList     = pidl;

						ShellExecuteEx(&ei);
					}
				}
				CoTaskMemFree(pidl);
			}
			psi->Release();
		}
		pfv2->Release();
	}

	return hr;
}

STDMETHODIMP CShellViewImpl::OnStateChange(IShellView * psv, ULONG uChange)
{
	::PrintLog(L"CShellViewImpl::OnStateChange, uChange = %d",uChange);
	if (uChange == CDBOSC_SELCHANGE)
	{
		//PostMessage(m_hWnd, KFD_SELCHANGE, 0, 0);
	}
	return S_OK;
}

STDMETHODIMP CShellViewImpl::IncludeObject(IShellView * psv, PCUITEMID_CHILD pidl)
{
	::PrintLog(L"CShellViewImpl::IncludeObject");
	return S_OK;
}



// ICommDlgBrowser2
HRESULT CShellViewImpl::Notify(IShellView *ppshv,DWORD dwNotifyType)
{
	//CDB2N_CONTEXTMENU_START
	//CDB2N_CONTEXTMENU_DONE
	return E_NOTIMPL;

	//switch (dwNotifyType)
	//{
	//case CDB2N_CONTEXTMENU_START:
	//	{
	//		m_hMenu = GetContextMenuForItems(1,NULL);
	//	}
	//	break;
	//case CDB2N_CONTEXTMENU_DONE:
	//	if( NULL != m_hMenu )
	//	{
	//		DestroyMenu(m_hMenu);
	//		m_hMenu = NULL;
	//	}
	//	break;
	//default:
	//	break;
	//}
	//return S_OK;
}
HRESULT CShellViewImpl::GetDefaultMenuText(IShellView *ppshv,LPWSTR pszText,int cchMax)
{
	return E_NOTIMPL;
}
HRESULT CShellViewImpl::GetViewFlags(DWORD *pdwFlags)
{
	*pdwFlags = CDB2GVF_SHOWALLFILES | CDB2GVF_ISFOLDERPICKER;
	return S_OK;
}

// ICommDlgBrowser3
HRESULT CShellViewImpl::OnColumnClicked(IShellView *ppshv,int iColumn)
{
	return E_NOTIMPL;
}
HRESULT CShellViewImpl::GetCurrentFilter(LPWSTR pszFileSpec,int cchFileSpec)
{
	return E_NOTIMPL;
}
HRESULT CShellViewImpl::OnPreViewCreated(IShellView *ppshv)
{
	return E_NOTIMPL;
}

#define MENU_OFFSET  1
#define MENU_MAX     100
HMENU CShellViewImpl::GetContextMenuForItems(UINT uItems,LPITEMIDLIST *aItems)
{
	LPCONTEXTMENU  pContextMenu = NULL;
	m_psfContainingFolder->GetUIObjectOf(m_hwndParent,
		uItems,
		(LPCITEMIDLIST*)aItems,
		IID_IContextMenu,
		NULL,
		(LPVOID*)&pContextMenu);

	if(pContextMenu)
	{
		m_hMenu = CreatePopupMenu();

		/*
		See if we are in Explore or Open mode. If the browser's tree is
		present, then we are in Explore mode.
		*/ 
		BOOL  fExplore = FALSE;
		HWND  hwndTree = NULL;
		if(SUCCEEDED(m_spShellBrowser->GetControlWindow(FCW_TREE,
			&hwndTree)) && hwndTree)
		{
			fExplore = TRUE;
		}

		if(m_hMenu && SUCCEEDED(pContextMenu->QueryContextMenu( m_hMenu,
			0,
			MENU_OFFSET,
			MENU_MAX,
			CMF_NORMAL | (fExplore ? CMF_EXPLORE : 0))))
		{

		}
		else
		{
			DestroyMenu(m_hMenu);
			m_hMenu = NULL;
		}
	}
	return m_hMenu;
}