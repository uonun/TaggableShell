#pragma once
#include "dllmain.h"
#include "TaggableShellEx.NsExt.CShellFolderImpl.h"
#include "TaggableShellEx.NsExt.CEnumIDListImpl.h"
//#include "TaggableShellEx.NsExt.CShellItemImpl_Tag.h"
#include "PidlMgr.h"

class CShellViewImpl:
	public IShellView,
	public IOleCommandTarget,
	public IServiceProvider,
	public ICommDlgBrowser
{
public:
	CShellViewImpl(void);

	// IUnknown
	IFACEMETHODIMP QueryInterface(REFIID riid, void ** ppv);
	IFACEMETHODIMP_(ULONG) AddRef();
	IFACEMETHODIMP_(ULONG) Release();

	// IServiceProvider
	STDMETHODIMP QueryService(REFGUID guidService, REFIID riid, void **ppv);

	// ICommDlgBrowser
	STDMETHODIMP OnDefaultCommand(IShellView * /* psv */);
	STDMETHODIMP OnStateChange(IShellView * /* psv */, ULONG uChange);
	STDMETHODIMP IncludeObject(IShellView * /* psv */, PCUITEMID_CHILD /* pidl */);

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
	HRESULT Init ( CShellFolderImpl* pContainingFolder );

	HRESULT GetItemFromView(IFolderView2 *pfv, int iItem, REFIID riid, void **ppv);

private:
	virtual ~CShellViewImpl(void);

	// IUnknown
	long _cRef;

	IExplorerBrowser *_peb;
	IResultsFolder *_prf;

	HWND m_hwndParent;
	HWND m_hWnd;

	CShellFolderImpl*      m_psfContainingFolder;
	FOLDERSETTINGS m_FolderSettings;
	IShellBrowser* m_spShellBrowser;

	CPidlMgr     m_PidlMgr;

	void FillList();

	// the ABSOLUTE pidl of current IShellView
	//ITEMIDLIST* _currentItem;

	// In C++ you must employ a free (C) function or a static
	// class member function as the thread entry-point-function.
	// Furthermore, _beginthreadex() demands that the thread
	// entry function signature take a single (void*) and returned
	// an unsigned.
	static unsigned __stdcall FillList_Tags(void * pThis);

};


LRESULT CALLBACK WndProc(
	HWND hwnd,        // handle to window
	UINT uMsg,        // message identifier
	WPARAM wParam,    // first message parameter
	LPARAM lParam);    // second message parameter
