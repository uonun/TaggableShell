#pragma once
#include "dllmain.h"
#include "Taghelper.h"
#include "NsExt.CShellViewImpl.h"
#include "NsExt.CEnumIDListImpl.h"
#include "PidlMgr.h"
#include <shidfact.h>
#include <propkeydef.h>
#include <propkey.h>

#include <propvarutil.h>  // for PropVariantToVariant
#pragma comment(lib, "Propsys.lib")	// for PropVariantToVariant

class CShellFolderImpl:
	public IExtractIcon
	,public IContextMenu
	,public IExplorerPaneVisibility
	,public IQueryInfo
	,public IShellFolder2	
	,public IPersistFolder
	,public IExplorerCommandProvider
	//,public IPropertyStore
{
public:
	CShellFolderImpl(void);
	~CShellFolderImpl(void);

	// IUnknown
	IFACEMETHODIMP QueryInterface(REFIID riid, void ** ppv);
	IFACEMETHODIMP_(ULONG) AddRef();
	IFACEMETHODIMP_(ULONG) Release();

	// IPersistFolder
	// @pidl: The address of the ITEMIDLIST (item identifier list) structure that specifies the absolute location of the folder.
	HRESULT Initialize(LPCITEMIDLIST pidl);

	// IPersist which is the base of IPersistFolder
	HRESULT GetClassID(CLSID *pClassID);

#pragma region IShellFolder
	// IShellFolder
	IFACEMETHODIMP ParseDisplayName(HWND hwnd, __in IBindCtx *pbc, __in PWSTR pszDisplayName, __inout ULONG *pchEaten, __deref_out PIDLIST_RELATIVE *ppidl, __inout ULONG *pdwAttributes);
	IFACEMETHODIMP EnumObjects(HWND hwnd, SHCONTF grfFlags, __deref_out IEnumIDList **ppenmIDList);
	IFACEMETHODIMP BindToObject(PCUIDLIST_RELATIVE pidl, __in IBindCtx *pbc, REFIID riid, __deref_out void **ppv);
	IFACEMETHODIMP BindToStorage(PCUIDLIST_RELATIVE pidl, __in IBindCtx *pbc, REFIID riid, __deref_out void **ppv);
	IFACEMETHODIMP CompareIDs(LPARAM lParam, PCUIDLIST_RELATIVE pidl1, PCUIDLIST_RELATIVE pidl2);
	IFACEMETHODIMP CreateViewObject(HWND hwndOwner, REFIID riid, __deref_out void **ppv);
	IFACEMETHODIMP GetAttributesOf(UINT cidl, __in_ecount_opt(cidl) PCUITEMID_CHILD_ARRAY rgpidl, __inout SFGAOF *rgfInOut);
	IFACEMETHODIMP GetUIObjectOf(HWND hwndOwner, UINT cidl, __in_ecount_opt(cidl) PCUITEMID_CHILD_ARRAY rgpidl, REFIID riid, __reserved UINT *rgfReserved, __deref_out void **ppv);
	IFACEMETHODIMP GetDisplayNameOf(PCUITEMID_CHILD pidl, SHGDNF uFlags, __out STRRET *psrName);
	IFACEMETHODIMP SetNameOf(HWND hwnd, PCUITEMID_CHILD pidl, LPCWSTR pszName, SHGDNF uFlags, __deref_out_opt PITEMID_CHILD *ppidlOut);

	// IShellFolder2
	IFACEMETHODIMP GetDefaultSearchGUID(__out GUID *pguid);
	IFACEMETHODIMP EnumSearches(__deref_out IEnumExtraSearch **ppenum);
	IFACEMETHODIMP GetDefaultColumn(DWORD dwRes, __out ULONG *plSort, __out ULONG *plDisplay);
	IFACEMETHODIMP GetDefaultColumnState(UINT iColumn, __out SHCOLSTATEF *pcsFlags);
	IFACEMETHODIMP GetDetailsEx(PCUITEMID_CHILD pidl, const PROPERTYKEY *pkey, __out VARIANT *pvar);
	IFACEMETHODIMP GetDetailsOf(__in_opt PCUITEMID_CHILD pidl, UINT iColumn, __out SHELLDETAILS *psd);
	IFACEMETHODIMP MapColumnToSCID(UINT iColumn, __out PROPERTYKEY *pkey);
#pragma endregion

	// IQueryInfo
	HRESULT GetInfoFlags(DWORD *pdwFlags) { pdwFlags = 0; return S_OK;} // This method is not currently used.
	HRESULT GetInfoTip(DWORD dwFlags, PWSTR *ppwszTip);

	// IExtractIcon
	HRESULT Extract(PCTSTR pszFile,UINT nIconIndex,HICON *phiconLarge,HICON *phiconSmall,UINT nIconSize);
	HRESULT GetIconLocation(UINT uFlags,PTSTR pszIconFile,UINT cchMax,int *piIndex,UINT *pwFlags);

	// IContextMenuCB
    //IFACEMETHODIMP CallBack(__in_opt IShellFolder *psf, HWND hwndOwner, __in_opt IDataObject *pdtobj, UINT uiMsg, WPARAM wParam, LPARAM lParam);

	// IContextMenu
	STDMETHODIMP GetCommandString(UINT_PTR idCmd, UINT uFlags, UINT* pwReserved, LPSTR pszName, UINT cchMax);
	STDMETHODIMP InvokeCommand(LPCMINVOKECOMMANDINFO pCmdInfo);
	STDMETHODIMP QueryContextMenu(HMENU hmenu, UINT uMenuIndex, UINT uidFirstCmd, UINT uidLastCmd, UINT uFlags);

	// IExplorerPaneVisibility
	HRESULT STDMETHODCALLTYPE GetPaneState(REFEXPLORERPANE ep, EXPLORERPANESTATE *peps);

	// IExplorerCommandProvider
	HRESULT STDMETHODCALLTYPE GetCommands(IUnknown *punkSite,REFIID riid,void **ppv);	
	HRESULT STDMETHODCALLTYPE GetCommand(REFGUID rguidCommandId,REFIID riid,void **ppv);

	//// IPropertyStore
	// HRESULT STDMETHODCALLTYPE GetCount(DWORD *cProps);	
	// HRESULT STDMETHODCALLTYPE GetAt(DWORD iProp,PROPERTYKEY *pkey);	
	// HRESULT STDMETHODCALLTYPE GetValue(REFPROPERTYKEY key,PROPVARIANT *pv);	
	// HRESULT STDMETHODCALLTYPE SetValue(REFPROPERTYKEY key,REFPROPVARIANT propvar) { return E_NOTIMPL;}	
	// HRESULT STDMETHODCALLTYPE Commit( void) { return E_NOTIMPL;}

	// Init function - call right after constructing a CShellFolderImpl object.
	HRESULT Init ( PIDLIST_ABSOLUTE pidl_perent, PIDLIST_RELATIVE pidl_current );
	BOOL IsShowTag();

	// current shell item data. (m_PIDLCurrent)
	MYPIDLDATA* CurrentShellItemData;

	// absolute path of the folder (m_pIDFolder)
	wchar_t FolderPath[MAX_PATH];

	// IPersistFolder
	PIDLIST_ABSOLUTE  m_pIDFolder;           // absolute location of the folder

	CTaghelper* pTagHelper;

private:

	// IUnknown
	long _cRef;

	// IContextMenu
	HMENU _hSubmenu;	
	CPidlMgr m_PidlMgr;
	PIDLIST_RELATIVE  m_PIDLCurrent;		 // relevant location to the folder

	// mutex lock
	HANDLE m_mutex;

	// get details for columns.
	HRESULT _GetPropertyForItem(PCUITEMID_CHILD pidl, REFPROPERTYKEY key, __out PROPVARIANT *ppropvar);
	HRESULT _GetDisplayName(PCUITEMID_CHILD pidl,REFPROPERTYKEY key, __out PROPVARIANT *ppropvar);    
    HRESULT _GetContainedItems(PCUITEMID_CHILD pidl,REFPROPERTYKEY key, __out PROPVARIANT *ppropvar);
    
	// Property helpers
    static const struct COLUMNINFO
    {
        REFPROPERTYKEY key;
        SHCOLSTATEF scsf;
        HRESULT (CShellFolderImpl::*pfnGetProperty)(PCUITEMID_CHILD,REFPROPERTYKEY, PROPVARIANT *);
    } c_rgColumnInfo[];
};