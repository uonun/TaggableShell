#pragma once
#include "dllmain.h"
#include "NsExt.CShellFolderImpl.h"
#include "NsExt.CEnumIDListImpl.h"
#include "CBackgroundThread.h"
#include "PidlMgr.h"

//#define IMPL_IShellFolderViewCB


class CShellViewImpl:
	public IShellView
	,public IOleCommandTarget
	,public IServiceProvider
	,public ICommDlgBrowser3
	,public IFolderView2
#ifdef IMPL_IShellFolderViewCB
	,public IShellFolderViewCB
#endif
	,public IContextMenuCB
	,public IBrowserFrameOptions
	//,public IPersistIDList
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

	// ICommDlgBrowser2
	HRESULT STDMETHODCALLTYPE Notify(IShellView *ppshv,DWORD dwNotifyType);
	HRESULT STDMETHODCALLTYPE GetDefaultMenuText(IShellView *ppshv,LPWSTR pszText,int cchMax);
	HRESULT STDMETHODCALLTYPE GetViewFlags(DWORD *pdwFlags);

	// ICommDlgBrowser3
	HRESULT STDMETHODCALLTYPE OnColumnClicked(IShellView *ppshv,int iColumn);
	HRESULT STDMETHODCALLTYPE GetCurrentFilter(LPWSTR pszFileSpec,int cchFileSpec);
	HRESULT STDMETHODCALLTYPE OnPreViewCreated(IShellView *ppshv);

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

#pragma region IFolderView
	// IFolderView
	HRESULT STDMETHODCALLTYPE GetCurrentViewMode( UINT *pViewMode);
	HRESULT STDMETHODCALLTYPE SetCurrentViewMode( UINT ViewMode);
	HRESULT STDMETHODCALLTYPE GetFolder( REFIID riid,void **ppv);
	HRESULT STDMETHODCALLTYPE Item( int iItemIndex,PITEMID_CHILD *ppidl);
	HRESULT STDMETHODCALLTYPE ItemCount( UINT uFlags,int *pcItems);
	HRESULT STDMETHODCALLTYPE Items( UINT uFlags,REFIID riid,void **ppv);
	HRESULT STDMETHODCALLTYPE GetSelectionMarkedItem( int *piItem);
	HRESULT STDMETHODCALLTYPE GetFocusedItem( int *piItem);
	HRESULT STDMETHODCALLTYPE GetItemPosition( PCUITEMID_CHILD pidl,POINT *ppt);
	HRESULT STDMETHODCALLTYPE GetSpacing(POINT *ppt);
	HRESULT STDMETHODCALLTYPE GetDefaultSpacing(POINT *ppt);
	HRESULT STDMETHODCALLTYPE GetAutoArrange( void);
	HRESULT STDMETHODCALLTYPE SelectItem(int iItem,DWORD dwFlags);
	HRESULT STDMETHODCALLTYPE SelectAndPositionItems(UINT cidl,PCUITEMID_CHILD_ARRAY apidl,POINT *apt,	DWORD dwFlags);

	// IFolderView2
	HRESULT STDMETHODCALLTYPE SetGroupBy(REFPROPERTYKEY key,BOOL fAscending);
	HRESULT STDMETHODCALLTYPE GetGroupBy( PROPERTYKEY *pkey,BOOL *pfAscending);
	DEPRECATED_HRESULT STDMETHODCALLTYPE SetViewProperty( PCUITEMID_CHILD pidl,REFPROPERTYKEY propkey,REFPROPVARIANT propvar){ return E_NOTIMPL; }
	DEPRECATED_HRESULT STDMETHODCALLTYPE GetViewProperty( PCUITEMID_CHILD pidl,REFPROPERTYKEY propkey,PROPVARIANT *ppropvar){ return E_NOTIMPL; }
	DEPRECATED_HRESULT STDMETHODCALLTYPE SetTileViewProperties( PCUITEMID_CHILD pidl,LPCWSTR pszPropList){ return E_NOTIMPL; }
	DEPRECATED_HRESULT STDMETHODCALLTYPE SetExtendedTileViewProperties( PCUITEMID_CHILD pidl,LPCWSTR pszPropList){ return E_NOTIMPL; }
	HRESULT STDMETHODCALLTYPE SetText( FVTEXTTYPE iType,LPCWSTR pwszText);
	HRESULT STDMETHODCALLTYPE SetCurrentFolderFlags(DWORD dwMask,DWORD dwFlags);
	HRESULT STDMETHODCALLTYPE GetCurrentFolderFlags(DWORD *pdwFlags);
	HRESULT STDMETHODCALLTYPE GetSortColumnCount(int *pcColumns);
	HRESULT STDMETHODCALLTYPE SetSortColumns(const SORTCOLUMN *rgSortColumns,int cColumns);
	HRESULT STDMETHODCALLTYPE GetSortColumns(SORTCOLUMN *rgSortColumns,int cColumns);
	HRESULT STDMETHODCALLTYPE GetItem(int iItem,REFIID riid,void **ppv);
	HRESULT STDMETHODCALLTYPE GetVisibleItem(int iStart,BOOL fPrevious,int *piItem);
	HRESULT STDMETHODCALLTYPE GetSelectedItem(int iStart,int *piItem);
	HRESULT STDMETHODCALLTYPE GetSelection(BOOL fNoneImpliesFolder,IShellItemArray **ppsia);
	HRESULT STDMETHODCALLTYPE GetSelectionState(PCUITEMID_CHILD pidl,DWORD *pdwFlags);
	HRESULT STDMETHODCALLTYPE InvokeVerbOnSelection(LPCSTR pszVerb);
	HRESULT STDMETHODCALLTYPE SetViewModeAndIconSize(FOLDERVIEWMODE uViewMode,int iImageSize);
	HRESULT STDMETHODCALLTYPE GetViewModeAndIconSize(FOLDERVIEWMODE *puViewMode,int *piImageSize);
	HRESULT STDMETHODCALLTYPE SetGroupSubsetCount(UINT cVisibleRows);
	HRESULT STDMETHODCALLTYPE GetGroupSubsetCount(UINT *pcVisibleRows);
	HRESULT STDMETHODCALLTYPE SetRedraw(BOOL fRedrawOn);
	HRESULT STDMETHODCALLTYPE IsMoveInSameFolder( void);
	HRESULT STDMETHODCALLTYPE DoRename( void);
#pragma endregion

#ifdef IMPL_IShellFolderViewCB
	// IShellFolderViewCB
	STDMETHOD(MessageSFVCB)(THIS_ UINT uMsg, WPARAM wParam, LPARAM lParam);
#endif

	// IContextMenuCB
    IFACEMETHODIMP CallBack(__in_opt IShellFolder *psf, HWND hwndOwner, __in_opt IDataObject *pdtobj, UINT uiMsg, WPARAM wParam, LPARAM lParam);

	// IBrowserFrameOptions
	HRESULT GetFrameOptions(
		BROWSERFRAMEOPTIONS dwMask,
		BROWSERFRAMEOPTIONS *pdwOptions
		);

	// Other stuff
	HRESULT Init ( CShellFolderImpl* pContainingFolder );

	// Fill list in background thread. used in CBackgroundThread.h
	HRESULT DoWorkAsyn(IResultsFolder *prf);

	HMENU GetContextMenuForItems(UINT uItems,LPITEMIDLIST *aItems);
private:
	~CShellViewImpl(void);

	// IUnknown
	long _cRef;

	HWND				m_hwndParent;
	HWND				m_hWnd;
	CShellFolderImpl*   m_psfContainingFolder;
	FOLDERSETTINGS		m_folderSettings;
	IShellBrowser*		m_spShellBrowser;	// the container of IExplorerBrowser, witch is in the Shell window.
	IStream*			m_pViewState;		
	IExplorerBrowser*	_peb;
	IResultsFolder*		_prf;
	CPidlMgr			m_PidlMgr;
	BOOL				m_isRefreshing;
	UINT				m_uUIState;	// current state
	HMENU				m_hMenu;
	DWORD				_dwCookie;	// A pointer to a DWORD that receives an implementation-defined value used for identification purposes. The calling application must keep track of this value for possible use in IProfferService::RevokeService.
	int					_currentIconSize;

	void InitExplorerBrowserColumns(IFolderView2* pfv2);
	BOOL IsShowTag();
	HRESULT GetItemFromView(IFolderView2 *pfv, int iItem, REFIID riid, void **ppv);

	void FillList(IResultsFolder* & prf);

	void HandleActivate(UINT uState);
	void HandleDeactivate();

	// mutex lock
	static HANDLE m_mutex;

};
