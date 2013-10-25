#pragma once
#include "dllmain.h"
#include "TaggableShellEx.Taghelper.h"
#include "PidlMgr.h"

class CShellFolderImpl:
	public IShellFolder,		// ShellFolder
	public IPersistFolder		// ShellFolder
{
public:
	CShellFolderImpl(void);
	~CShellFolderImpl(void);

	// IUnknown
	IFACEMETHODIMP QueryInterface(REFIID riid, void ** ppv);
	IFACEMETHODIMP_(ULONG) AddRef();
	IFACEMETHODIMP_(ULONG) Release();

	// IShellExtInit for ContextMenu, PropertyPage
	STDMETHODIMP Initialize(LPCITEMIDLIST pIDFolder, 
		IDataObject *pDataObj, 
		HKEY hRegKey);

	// IPersistFolder
	// @pidl: The address of the ITEMIDLIST (item identifier list) structure that specifies the absolute location of the folder.
	HRESULT Initialize(LPCITEMIDLIST pidl);

	// IPersist which is the base of IPersistFolder
	HRESULT GetClassID(
		CLSID *pClassID
		);

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
		){	return S_OK;	};

	HRESULT CompareIDs(
		LPARAM lParam,
		PCUIDLIST_RELATIVE pidl1,
		PCUIDLIST_RELATIVE pidl2
		){	return S_OK;	};

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
		){	return S_OK;	};
#pragma endregion

	// Init function - call right after constructing a CShellFolderImpl object.
	HRESULT _init ( CShellFolderImpl* pParentFolder, LPCITEMIDLIST pidl )
	{
		m_pParentFolder = pParentFolder;
		m_PIDLCurrent = (LPITEMIDLIST)pidl;
		return S_OK;
	}

	HWND _hdlg;

	CTaghelper TagHelper;

private:

	// IUnknown
	long _cRef;

	// IPersistFolder
	LPITEMIDLIST  m_pIDFolder;           // absolute location of the folder
	LPITEMIDLIST       m_PIDLCurrent;	 // relevant location to the folder

	CShellFolderImpl*  m_pParentFolder;
	CPidlMgr m_PidlMgr;
};