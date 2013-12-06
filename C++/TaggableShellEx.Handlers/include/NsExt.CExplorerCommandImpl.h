#pragma once
#include "dllmain.h"
class CShellFolderImpl;

class CExplorerCommandImpl:
	public IExplorerCommand
{
public:
	CExplorerCommandImpl(void);
	~CExplorerCommandImpl(void);

	// IUnknown
	IFACEMETHODIMP QueryInterface(REFIID riid, void ** ppv);
	IFACEMETHODIMP_(ULONG) AddRef();
	IFACEMETHODIMP_(ULONG) Release();

	// IExplorerCommand
	STDMETHOD(GetTitle)(IShellItemArray* psiItemArray, LPWSTR* ppszName);        
	STDMETHOD(GetIcon)(IShellItemArray* psiItemArray, LPWSTR* ppszIcon);        
	STDMETHOD(GetToolTip)(IShellItemArray* psiItemArray, LPWSTR* ppszInfotip);        
	STDMETHOD(GetCanonicalName)(GUID* pguidCommandName);        
	STDMETHOD(GetState)(IShellItemArray* psiItemArray, BOOL fOkToBeSlow, EXPCMDSTATE* pCmdState);        
	STDMETHOD(Invoke)(IShellItemArray* psiItemArray, IBindCtx* pbc);        
	STDMETHOD(GetFlags)(EXPCMDFLAGS* pFlags);        
	STDMETHOD(EnumSubCommands)(IEnumExplorerCommand* *ppEnum);

	HRESULT Init(CShellFolderImpl* pFolder, const MENUITEMINFO& mii,const UINT & iconID);
	MENUITEMINFO m_mii;// Information about command

private:
	CShellFolderImpl* m_pFolder;// Reference to command provider
	WCHAR m_wszTitle[100];// Title of comamnd (MENUITEMINFO buffer)

	// IUnknown
	long _cRef;

	UINT _iconID;

	// Static members
	static GUID GetCanonicalName(UINT wID);
	static UINT GetMenuID(GUID guid);
};