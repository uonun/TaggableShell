#pragma once
#include "../include/TaggableShellEx.NsExt.CEnumIDListImpl.h"

CEnumIDListImpl::CEnumIDListImpl(void):
	_cRef(1) // IUnknown
	,_currentIdx(0)
{

}


CEnumIDListImpl::~CEnumIDListImpl(void)
{
	_items.clear();
}

// IUnknown
IFACEMETHODIMP CEnumIDListImpl::QueryInterface(REFIID riid, void ** ppv)
{
	static const QITAB qit[] =
	{
		QITABENT(CEnumIDListImpl, IEnumIDList),
		{0},
	};
	return QISearch(this, qit, riid, ppv);
}

IFACEMETHODIMP_(ULONG) CEnumIDListImpl::AddRef()
{
	return InterlockedIncrement(&_cRef);
}

IFACEMETHODIMP_(ULONG) CEnumIDListImpl::Release()
{
	long cRef = InterlockedDecrement(&_cRef);
	if (cRef == 0)
	{
		delete this;
	}
	return cRef;
}

void CEnumIDListImpl::Init(PIDLIST_ABSOLUTE parent,vector<MYPIDLDATA> items)
{
	_parent = parent;

	_items.clear();
	_items = items;
}

// IEnumIDList
#pragma region IEnumIDList
HRESULT CEnumIDListImpl::Clone(
	IEnumIDList **ppenum
	)
{
	return S_OK;
}


HRESULT CEnumIDListImpl::Next(
	ULONG celt,
	LPITEMIDLIST *rgelt,
	ULONG *pceltFetched
	)
{
	*rgelt = NULL;

	LPMYPIDLDATA   pidlOut = NULL;
	//Calculate the size of the MYPIDLDATA structure.
	USHORT         uSize = sizeof(MYPIDLDATA);
	ULONG n = 0;


	if ( _currentIdx >= _items.size())
	{
		*pceltFetched = 0;
		return S_FALSE;
	}

	// The implementation must allocate these item identifiers using CoTaskMemAlloc. The calling application is responsible for freeing the item identifiers using CoTaskMemFree.
	// plus the terminator
	pidlOut = (LPMYPIDLDATA)CoTaskMemAlloc(uSize  * celt + sizeof(USHORT));

	if(pidlOut)
	{
		*rgelt = (LPITEMIDLIST)pidlOut;

		while ( _currentIdx < _items.size())
		{
			auto next = _items[_currentIdx++];

			auto pidl = m_PidlMgr.Create((LPCMYPIDLDATA)&next);
			UINT cbSrc = m_PidlMgr.GetSize(pidl);
			CopyMemory ( pidlOut, pidl, cbSrc );
			m_PidlMgr.Delete(pidl);

			////Assign values to the members of the MYPIDLDATA structure
			////that is the PIDL's first SHITEMID structure
			//pidlOut->cb = uSize;
			//pidlOut->Type = next.Type;
			//pidlOut->TagID = next.TagID;
			//pidlOut->TagIdx = next.TagIdx;
			//pidlOut->UseCount = next.UseCount;
			//StringCbCopyW(pidlOut->wszDisplayName,sizeof(pidlOut->wszDisplayName), next.wszDisplayName);
			//StringCbCopyW(pidlOut->wszRemark,sizeof(pidlOut->wszRemark), next.wszRemark);

			//Advance the pointer to the start of the next SHITEMID structure.
			pidlOut = (LPMYPIDLDATA)((LPBYTE)pidlOut + pidlOut->cb);

			if ( ++n == celt )
			{
				//Create the terminating null character by setting cb to 0.
				pidlOut->cb = 0;
				break;
			}
		}

	}

	// try to return absolute pidl.
	//auto d = m_PidlMgr.Copy(_parent);
	//*rgelt = ILAppendID(d,(SHITEMID*)pidlOut,true);

	*pceltFetched = n;

	return *pceltFetched == celt ? S_OK : S_FALSE;
}

HRESULT CEnumIDListImpl::Reset()
{ 
	_currentIdx = 0;
	return S_OK;
}


HRESULT CEnumIDListImpl::Skip(
	ULONG celt
	)
{
	_currentIdx += celt;
	return S_OK;
}

#pragma endregion
