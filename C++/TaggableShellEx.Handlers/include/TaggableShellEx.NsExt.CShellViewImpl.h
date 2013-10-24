#pragma once
#include "dllmain.h"
#include "TaggableShellEx.NsExt.CShellFolderImpl.h"
#include "TaggableShellEx.NsExt.CEnumIDListImpl.h"
#include "PidlMgr.h"

class CShellViewImpl:
	public IShellView,
	public IOleCommandTarget
{
public:
	CShellViewImpl(void);
	virtual ~CShellViewImpl(void);

	// IUnknown
	IFACEMETHODIMP QueryInterface(REFIID riid, void ** ppv);
	IFACEMETHODIMP_(ULONG) AddRef();
	IFACEMETHODIMP_(ULONG) Release();

	// IOleWindow
	STDMETHOD(GetWindow)(HWND* phwnd);

	STDMETHOD(ContextSensitiveHelp)(BOOL){ return E_NOTIMPL; }

	// IShellView methods
	STDMETHOD(CreateViewWindow)(LPSHELLVIEW, LPCFOLDERSETTINGS, LPSHELLBROWSER, LPRECT, HWND*);
	STDMETHOD(DestroyViewWindow)();
	STDMETHOD(GetCurrentInfo)(LPFOLDERSETTINGS);
	STDMETHOD(Refresh)();
	STDMETHOD(UIActivate)(UINT);

	STDMETHOD(AddPropertySheetPages)(DWORD, LPFNADDPROPSHEETPAGE, LPARAM)
	{ return E_NOTIMPL; }
	STDMETHOD(EnableModeless)(BOOL)
	{ return E_NOTIMPL; }
	STDMETHOD(GetItemObject)(UINT, REFIID, void**)
	{ return E_NOTIMPL; }
	STDMETHOD(SaveViewState)()
	{ return E_NOTIMPL; }
	STDMETHOD(SelectItem)(LPCITEMIDLIST, UINT)
	{ return E_NOTIMPL; }
	STDMETHOD(TranslateAccelerator)(LPMSG)
	{ return E_NOTIMPL; }

	// IOleCommandTarget methods
	STDMETHOD(QueryStatus)(const GUID*, ULONG, OLECMD prgCmds[], OLECMDTEXT*);
	STDMETHOD(Exec)(const GUID*, DWORD, DWORD, VARIANTARG*, VARIANTARG*);

	// Other stuff
	HRESULT _init ( CShellFolderImpl* pContainingFolder )
	{
		m_psfContainingFolder = pContainingFolder;

		if ( NULL != m_psfContainingFolder )
			m_psfContainingFolder->AddRef();

		return S_OK;
	}

private:

	// IUnknown
	long _cRef;

	HWND m_hwndParent;
	HWND m_hWnd;

	CShellFolderImpl*      m_psfContainingFolder;
	FOLDERSETTINGS m_FolderSettings;
	IShellBrowser* m_spShellBrowser;
	
	CPidlMgr     m_PidlMgr;

	void FillList();

};

