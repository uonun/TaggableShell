#pragma once
#include "dllmain.h"
#include "NsExt.CShellFolderImpl.h"
#include "NsExt.CEnumIDListImpl.h"
//#include "NsExt.CShellItemImpl_Tag.h"
#include "PidlMgr.h"

class CShellViewImpl:
	public IShellView,
	public IOleCommandTarget,
	public IServiceProvider,
	public ICommDlgBrowser,
	public IFolderView
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
	STDMETHOD(AddPropertySheetPages)(DWORD, LPFNADDPROPSHEETPAGE, LPARAM);
	STDMETHOD(EnableModeless)(BOOL);
	STDMETHOD(GetItemObject)(UINT, REFIID, void**);
	STDMETHOD(SaveViewState)();
	STDMETHOD(SelectItem)(LPCITEMIDLIST, UINT);
	STDMETHOD(TranslateAccelerator)(LPMSG);

	// IOleCommandTarget methods
	STDMETHOD(QueryStatus)(const GUID*, ULONG, OLECMD prgCmds[], OLECMDTEXT*);
	STDMETHOD(Exec)(const GUID*, DWORD, DWORD, VARIANTARG*, VARIANTARG*);

	// IFolderView
	HRESULT STDMETHODCALLTYPE GetCurrentViewMode( 
		UINT *pViewMode);

	HRESULT STDMETHODCALLTYPE SetCurrentViewMode( 
		UINT ViewMode);

	HRESULT STDMETHODCALLTYPE GetFolder( 
		REFIID riid,
		void **ppv);

	HRESULT STDMETHODCALLTYPE Item( 
		int iItemIndex,
		PITEMID_CHILD *ppidl);

	HRESULT STDMETHODCALLTYPE ItemCount( 
		UINT uFlags,
		int *pcItems);

	HRESULT STDMETHODCALLTYPE Items( 
		UINT uFlags,
		REFIID riid,
		void **ppv);

	HRESULT STDMETHODCALLTYPE GetSelectionMarkedItem( 
		int *piItem);

	HRESULT STDMETHODCALLTYPE GetFocusedItem( 
		int *piItem);

	HRESULT STDMETHODCALLTYPE GetItemPosition( 
		PCUITEMID_CHILD pidl,
		POINT *ppt);

	HRESULT STDMETHODCALLTYPE GetSpacing( 
		POINT *ppt);

	HRESULT STDMETHODCALLTYPE GetDefaultSpacing( 
		POINT *ppt);

	HRESULT STDMETHODCALLTYPE GetAutoArrange( void);

	HRESULT STDMETHODCALLTYPE SelectItem( 
		int iItem,
		DWORD dwFlags);

	HRESULT STDMETHODCALLTYPE SelectAndPositionItems( 
		UINT cidl,
		PCUITEMID_CHILD_ARRAY apidl,
		POINT *apt,
		DWORD dwFlags);

	// Other stuff
	HRESULT Init ( CShellFolderImpl* pContainingFolder );

private:
	~CShellViewImpl(void);

	// IUnknown
	long _cRef;

	HWND				m_hwndParent;
	HWND				m_hWnd;
	CShellFolderImpl*   m_psfContainingFolder;
	FOLDERSETTINGS		m_FolderSettings;
	IShellBrowser*		m_spShellBrowser;	// the container of IExplorerBrowser, witch is in the Shell window.
	IStream*			m_pViewState;		
	IExplorerBrowser*	_peb;
	IResultsFolder*		_prf;
	CPidlMgr			m_PidlMgr;
	BOOL				_isRefreshing;
	UINT				m_uUIState;	// current state
	HMENU				m_hMenu;

	void FillList();
	void InitExplorerBrowserColumns(IFolderView2* pfv2);
	BOOL IsShowTag();
	HRESULT GetItemFromView(IFolderView2 *pfv, int iItem, REFIID riid, void **ppv);

	// In C++ you must employ a free (C) function or a static
	// class member function as the thread entry-point-function.
	// Furthermore, _beginthreadex() demands that the thread
	// entry function signature take a single (void*) and returned
	// an unsigned.
	static unsigned __stdcall FillList_Asyn(void * pThis);
	void HandleActivate(UINT uState);
	void HandleDeactivate();
};