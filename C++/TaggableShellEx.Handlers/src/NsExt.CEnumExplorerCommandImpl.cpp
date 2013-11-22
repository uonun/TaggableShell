#include "..\include\NsExt.CEnumExplorerCommandImpl.h"


CEnumExplorerCommandImpl::CEnumExplorerCommandImpl(void):
	_cRef(1) // IUnknown
	,_currentIdx(0)
{
}


CEnumExplorerCommandImpl::~CEnumExplorerCommandImpl(void)
{
}

// IUnknown
IFACEMETHODIMP CEnumExplorerCommandImpl::QueryInterface(REFIID riid, void ** ppv)
{
	static const QITAB qit[] =
	{
		QITABENT(CEnumExplorerCommandImpl, IEnumExplorerCommand),
		{0},
	};
	return QISearch(this, qit, riid, ppv);
}

IFACEMETHODIMP_(ULONG) CEnumExplorerCommandImpl::AddRef()
{
	return InterlockedIncrement(&_cRef);
}

IFACEMETHODIMP_(ULONG) CEnumExplorerCommandImpl::Release()
{
	long cRef = InterlockedDecrement(&_cRef);
	if (cRef == 0)
	{
		delete this;
	}
	return cRef;
}


HRESULT CEnumExplorerCommandImpl::Clone(
	IEnumExplorerCommand **ppenum
	)
{
	return E_NOTIMPL;
}


HRESULT CEnumExplorerCommandImpl::Next(
	ULONG celt,
	IExplorerCommand **pUICommand,
	ULONG *pceltFetched
	)
{
	*pUICommand = NULL;
	return E_NOTIMPL;
	//return *pceltFetched == celt ? S_OK : S_FALSE;
}

HRESULT CEnumExplorerCommandImpl::Reset()
{ 
	_currentIdx = 0;
	return S_OK;
}


HRESULT CEnumExplorerCommandImpl::Skip(
	ULONG celt
	)
{
	_currentIdx += celt;
	return S_OK;
}
