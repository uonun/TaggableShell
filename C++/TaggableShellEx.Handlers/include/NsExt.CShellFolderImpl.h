#pragma once
#include "dllmain.h"
#include "Taghelper.h"
#include "NsExt.CShellViewImpl.h"
#include "NsExt.CEnumIDListImpl.h"
#include "PidlMgr.h"

class CShellFolderImpl:
	public IContextMenu,
	public IQueryInfo,
	public IShellFolder,	
	public IPersistFolder	
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

	// IShellFolder
#pragma region IShellFolder
	HRESULT BindToObject(
		PCUIDLIST_RELATIVE pidl,
		IBindCtx *pbc,
		REFIID riid,
		void **ppvOut
		);

	HRESULT BindToStorage(
		PCUIDLIST_RELATIVE pidl,
		IBindCtx *pbc,
		REFIID riid,
		void **ppvOut
		);

	HRESULT CompareIDs(
		LPARAM lParam,
		PCUIDLIST_RELATIVE pidl1,
		PCUIDLIST_RELATIVE pidl2
		);

	HRESULT CreateViewObject(
		HWND hwndOwner,
		REFIID riid,
		void **ppv
		);

	HRESULT EnumObjects(
		HWND hwndOwner,
		SHCONTF grfFlags,
		IEnumIDList **ppenumIDList
		);

	HRESULT GetAttributesOf(
		UINT cidl,
		LPCITEMIDLIST *apidl,
		SFGAOF *rgfInOut
		);

	HRESULT GetDisplayNameOf(
		PCUITEMID_CHILD pidl,
		SHGDNF uFlags,
		STRRET *pName
		);

	HRESULT GetUIObjectOf(
		HWND hwndOwner,
		UINT cidl,
		PCUITEMID_CHILD_ARRAY apidl,
		REFIID riid,
		UINT *rgfReserved,
		void **ppv
		);

	HRESULT ParseDisplayName(
		HWND hwnd,
		IBindCtx *pbc,
		LPWSTR pszDisplayName,
		ULONG *pchEaten,
		PIDLIST_RELATIVE *ppidl,
		ULONG *pdwAttributes
		);

	HRESULT SetNameOf(
		HWND hwndOwner,
		PCUITEMID_CHILD pidl,
		LPCWSTR pszName,
		SHGDNF uFlags,
		PITEMID_CHILD *ppidlOut
		);
#pragma endregion

	// IQueryInfo
	HRESULT GetInfoFlags(DWORD *pdwFlags) { pdwFlags = 0; return S_OK;} // This method is not currently used.
	HRESULT GetInfoTip(DWORD dwFlags, PWSTR *ppwszTip);

	// IContextMenu
	STDMETHODIMP GetCommandString(UINT_PTR idCmd, UINT uFlags, UINT* pwReserved, LPSTR pszName, UINT cchMax);
	STDMETHODIMP InvokeCommand(LPCMINVOKECOMMANDINFO pCmdInfo);
	STDMETHODIMP QueryContextMenu(HMENU hmenu, UINT uMenuIndex, UINT uidFirstCmd, UINT uidLastCmd, UINT uFlags);


	// Init function - call right after constructing a CShellFolderImpl object.
	HRESULT Init ( PIDLIST_ABSOLUTE pidl_perent, PIDLIST_RELATIVE pidl_current )
	{
		m_pIDFolder = pidl_perent;
		m_PIDLCurrent = pidl_current;
		return S_OK;
	}

	CTaghelper TagHelper;

	// IPersistFolder
	PIDLIST_ABSOLUTE  m_pIDFolder;           // absolute location of the folder
	PIDLIST_RELATIVE  m_PIDLCurrent;		 // relevant location to the folder

private:

	// IUnknown
	long _cRef;

	// IContextMenu
	HMENU _hSubmenu;	

	// mark up current instance is initialized.
	// for by USER actions, the IPersistFolder::Initialize(LPCITEMIDLIST) will be called only if:
	//		click the Folder on the tree-view
	//		open the Folder on the IExplorerBrowser.
	// and will NOT be called while:
	//		open the Tag in Folder on the IExplorerBrowser.
	BOOL _initialized;

	wchar_t _FolderPath[MAX_PATH];
	CPidlMgr m_PidlMgr;
	IShellView* _pView;
};