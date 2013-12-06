#pragma once
#include "dllmain.h"
#include "NsExt.CExplorerCommandImpl.h"
#include "NsExt.CShellFolderImpl.h"

#define MAXCOUNT_CMD	10

class CEnumExplorerCommandImpl:
	public IEnumExplorerCommand
{
public:
	CEnumExplorerCommandImpl(CShellFolderImpl* folder);

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

	CShellFolderImpl* Folder;

private:
	~CEnumExplorerCommandImpl(void);

	void Init();

	// IUnknown
	long _cRef;

	CExplorerCommandImpl** _cmds;
	UINT _cmdCount;
	UINT _currentIdx;
};

