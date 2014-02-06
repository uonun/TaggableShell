#pragma once
#include "..\include\NsExt.CEnumExplorerCommandImpl.h"

CEnumExplorerCommandImpl::CEnumExplorerCommandImpl(CShellFolderImpl* folder):
	_cRef(1) // IUnknown
	,_currentIdx(0)
	,_cmdCount(0)
	,Folder(NULL)
{
	// allocate memory for cmds, freed in ~ctor.
	_cmds = (CExplorerCommandImpl**)CoTaskMemAlloc(sizeof(CExplorerCommandImpl*) * MAXCOUNT_CMD);

	Folder = folder;
	Folder->AddRef();

	Init();
}


CEnumExplorerCommandImpl::~CEnumExplorerCommandImpl(void)
{
	if( _cmds != NULL )
	{
		// cmds in "_cmds" are just pointers,
		// the real instances of them created in Init() by "new", need to release them manually.
		for (UINT i = 0; i < _cmdCount; i++)
		{
			_cmds[i]->Release();
		}
		CoTaskMemFree(_cmds);
	}

	_cmdCount = 0;
	Folder->Release();
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

#pragma region Initialize

enum CMD_FLAG
{
	// show the command in the view for tags list
	ForFolder = 0x1,
	// show the command in the view for items list of specific tag.
	ForItems = 0x2,
	BOTH = ForFolder | ForItems
};
struct CMD_OBJECT_INIT
{
	UINT cmdId;
	UINT cmdText_resourceid;
	UINT flags;	// CMD_FLAG
	UINT icon_resourceid;
};
// add commands supported by this module here
const CMD_OBJECT_INIT c_cmds[] =
{
	{ CMD_CMDBAR_NEWTAG,	IDS_CMDBAR_NEWTAG,		ForFolder,	IDI_ICON },
	{ CMD_CMDBAR_MODIFYTAG, IDS_CMDBAR_MODIFYTAG,	ForFolder,	IDI_ICON },
	{ CMD_CMDBAR_DELTAG,	IDS_CMDBAR_DELTAG,		ForFolder,	IDI_ICON },
	{ CMD_CMDBAR_SETTINGS,	IDS_CMDBAR_SETTINGS,	BOTH ,		IDI_ICON },
	{ CMD_CMDBAR_ABOUT,		IDS_CMDBAR_ABOUT,		BOTH ,		IDI_ICON },
};
void CEnumExplorerCommandImpl::Init()
{
	MENUITEMINFO m = {sizeof(MENUITEMINFO),MIIM_STRING | MIIM_ID | MIIM_BITMAP, MFT_STRING, MFS_ENABLED};
	BOOL showTag = Folder->IsShowTagList();
	for (int i = 0; i < ARRAYSIZE(c_cmds); i++)
	{
		if ( (showTag && (c_cmds[i].flags & ForFolder))	|| (!showTag && (c_cmds[i].flags & ForItems) )	) 
		{
			// to make sure there is enough memory to store the commands in _cmds.
			_ASSERT_EXPR( _cmdCount < MAXCOUNT_CMD ,L"not supporting so many command. please change the value of MAXCOUNT_CMD.");

			m.wID = c_cmds[i].cmdId;
			m.dwTypeData = ::MyLoadString(c_cmds[i].cmdText_resourceid);
			m.cch = wcslen(m.dwTypeData);
			m.hbmpItem = LoadBitmap(g_hInst,MAKEINTRESOURCE(c_cmds[i].icon_resourceid));

			// new instance of cmd, released in ~ctor.
			CExplorerCommandImpl *c = new CExplorerCommandImpl();
			c->Init(Folder,m,c_cmds[i].icon_resourceid);

			_cmds[_cmdCount++] = c;
		}
	}
}

#pragma endregion

// IEnumExplorerCommand
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
	if ( _cmds != NULL && _currentIdx < _cmdCount )
	{
		// HACK: considering the celt = 1.
		ULONG n = 1;
		_cmds[_currentIdx++]->QueryInterface(IID_PPV_ARGS(pUICommand));
		pceltFetched = &n;
		return *pceltFetched == celt ? S_OK : S_FALSE;
	}
	else
	{
		*pUICommand = NULL;
		pceltFetched = NULL;
		return S_FALSE;
	}
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
