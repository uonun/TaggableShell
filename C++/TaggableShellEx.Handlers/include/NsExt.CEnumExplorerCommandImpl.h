#pragma once
#include "dllmain.h"

class CEnumExplorerCommandImpl:
	public IEnumExplorerCommand
{
public:
	CEnumExplorerCommandImpl(void);
	
	// IUnknown
	IFACEMETHODIMP QueryInterface(REFIID riid, void ** ppv);
	IFACEMETHODIMP_(ULONG) AddRef();
	IFACEMETHODIMP_(ULONG) Release();
	
	// IEnumExplorerCommand
#pragma region IEnumExplorerCommand
	HRESULT Clone(
		IEnumExplorerCommand **ppenum
		);

	HRESULT Next(
		ULONG celt,
		IExplorerCommand **pUICommand,
		ULONG *pceltFetched
		);

	HRESULT Reset();

	HRESULT Skip(
		ULONG celt
		);
#pragma endregion

private:
	~CEnumExplorerCommandImpl(void);

	// IUnknown
	long _cRef;

	UINT _currentIdx;
};

