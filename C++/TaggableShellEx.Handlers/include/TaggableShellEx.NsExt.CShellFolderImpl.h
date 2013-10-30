#pragma once
#include "dllmain.h"
#include "TaggableShellEx.Taghelper.h"
#include "TaggableShellEx.NsExt.CShellViewImpl.h"
#include "TaggableShellEx.NsExt.CEnumIDListImpl.h"
#include "PidlMgr.h"

class CShellFolderImpl:
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
	BOOL _expanded_in_tree;

	CPidlMgr m_PidlMgr;
};