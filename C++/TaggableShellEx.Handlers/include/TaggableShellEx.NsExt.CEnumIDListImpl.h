#pragma once
#include "dllmain.h"
#include "PidlMgr.h"

class CEnumIDListImpl:
	public IEnumIDList
{
public:
	CEnumIDListImpl(void);
	virtual ~CEnumIDListImpl(void);

	// IUnknown
	IFACEMETHODIMP QueryInterface(REFIID riid, void ** ppv);
	IFACEMETHODIMP_(ULONG) AddRef();
	IFACEMETHODIMP_(ULONG) Release();

	void Init(vector<MYPIDLDATA>);

	// IEnumIDList
#pragma region IEnumIDList
	HRESULT Clone(
		IEnumIDList **ppenum
		);

	HRESULT Next(
		ULONG celt,
		LPITEMIDLIST *rgelt,
		ULONG *pceltFetched
		);

	HRESULT Reset();

	HRESULT Skip(
		ULONG celt
		);
#pragma endregion

private:

	// IUnknown
	long _cRef;

	UINT _currentIdx;
	vector<MYPIDLDATA> _items;
};

