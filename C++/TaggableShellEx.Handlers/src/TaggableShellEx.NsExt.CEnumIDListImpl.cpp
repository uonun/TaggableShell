#include "..\include\TaggableShellEx.NsExt.CEnumIDListImpl.h"

CEnumIDListImpl::CEnumIDListImpl(void):
	_cRef(1) // IUnknown
	,_currentIdx(0)
{

}


CEnumIDListImpl::~CEnumIDListImpl(void)
{
	//Items.clear();
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

void CEnumIDListImpl::Init(vector<MYPIDLDATA> items)
{
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
	pidlOut = (LPMYPIDLDATA)CoTaskMemAlloc(uSize + sizeof(USHORT));

	if(pidlOut)
	{
		*rgelt = (LPITEMIDLIST)pidlOut;

		while ( _currentIdx < _items.size())
		{
			auto next = _items[_currentIdx++];

			//Assign values to the members of the MYPIDLDATA structure
			//that is the PIDL's first SHITEMID structure
			pidlOut->cb = uSize;
			pidlOut->TagIdx = next.TagIdx;
			pidlOut->UseCount = next.UseCount;
			StringCbCopyW(pidlOut->wszDisplayName, 
				sizeof(pidlOut->wszDisplayName), next.wszDisplayName);

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
