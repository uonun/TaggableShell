using System;
using System.Collections.Generic;
using System.Text;
using System.Runtime.InteropServices;
using udnz.com.WinNative.Enums;

namespace udnz.com.WinNative.Interfaces
{
    [ComImport]
    [InterfaceType(ComInterfaceType.InterfaceIsIUnknown)]
    [Guid("11a66efa-382e-451a-9234-1e0e12ef3085")]
    public interface IShellLibrary
    {
        
        //  virtual HRESULT STDMETHODCALLTYPE LoadLibraryFromItem( 
        //    /* [in] */ __RPC__in_opt IShellItem *psiLibrary,
        //    /* [in] */ DWORD grfMode) = 0;
        
        //virtual HRESULT STDMETHODCALLTYPE LoadLibraryFromKnownFolder( 
        //    /* [in] */ __RPC__in REFKNOWNFOLDERID kfidLibrary,
        //    /* [in] */ DWORD grfMode) = 0;
        
        //virtual HRESULT STDMETHODCALLTYPE AddFolder( 
        //    /* [in] */ __RPC__in_opt IShellItem *psiLocation) = 0;
        
        //virtual HRESULT STDMETHODCALLTYPE RemoveFolder( 
        //    /* [in] */ __RPC__in_opt IShellItem *psiLocation) = 0;
        
        //virtual HRESULT STDMETHODCALLTYPE GetFolders( 
        //    /* [in] */ LIBRARYFOLDERFILTER lff,
        //    /* [in] */ __RPC__in REFIID riid,
        //    /* [iid_is][out] */ __RPC__deref_out_opt void **ppv) = 0;
        
        //virtual HRESULT STDMETHODCALLTYPE ResolveFolder( 
        //    /* [in] */ __RPC__in_opt IShellItem *psiFolderToResolve,
        //    /* [in] */ DWORD dwTimeout,
        //    /* [in] */ __RPC__in REFIID riid,
        //    /* [iid_is][out] */ __RPC__deref_out_opt void **ppv) = 0;
        
        //virtual HRESULT STDMETHODCALLTYPE GetDefaultSaveFolder( 
        //    /* [in] */ DEFAULTSAVEFOLDERTYPE dsft,
        //    /* [in] */ __RPC__in REFIID riid,
        //    /* [iid_is][out] */ __RPC__deref_out_opt void **ppv) = 0;
        
        //virtual HRESULT STDMETHODCALLTYPE SetDefaultSaveFolder( 
        //    /* [in] */ DEFAULTSAVEFOLDERTYPE dsft,
        //    /* [in] */ __RPC__in_opt IShellItem *psi) = 0;
        
        //virtual HRESULT STDMETHODCALLTYPE GetOptions( 
        //    /* [out] */ __RPC__out LIBRARYOPTIONFLAGS *plofOptions) = 0;
        
        //virtual HRESULT STDMETHODCALLTYPE SetOptions( 
        //    /* [in] */ LIBRARYOPTIONFLAGS lofMask,
        //    /* [in] */ LIBRARYOPTIONFLAGS lofOptions) = 0;
        
        //virtual HRESULT STDMETHODCALLTYPE GetFolderType( 
        //    /* [out] */ __RPC__out FOLDERTYPEID *pftid) = 0;
        
        //virtual HRESULT STDMETHODCALLTYPE SetFolderType( 
        //    /* [in] */ __RPC__in REFFOLDERTYPEID ftid) = 0;
        
        //virtual HRESULT STDMETHODCALLTYPE GetIcon( 
        //    /* [string][out] */ __RPC__deref_out_opt_string LPWSTR *ppszIcon) = 0;
        
        //virtual HRESULT STDMETHODCALLTYPE SetIcon( 
        //    /* [string][in] */ __RPC__in_string LPCWSTR pszIcon) = 0;
        
        //virtual HRESULT STDMETHODCALLTYPE Commit( void) = 0;
        
        //virtual HRESULT STDMETHODCALLTYPE Save( 
        //    /* [in] */ __RPC__in_opt IShellItem *psiFolderToSaveIn,
        //    /* [string][in] */ __RPC__in_string LPCWSTR pszLibraryName,
        //    /* [in] */ LIBRARYSAVEFLAGS lsf,
        //    /* [out] */ __RPC__deref_out_opt IShellItem **ppsiSavedTo) = 0;
        
        //virtual HRESULT STDMETHODCALLTYPE SaveInKnownFolder( 
        //    /* [in] */ __RPC__in REFKNOWNFOLDERID kfidToSaveIn,
        //    /* [string][in] */ __RPC__in_string LPCWSTR pszLibraryName,
        //    /* [in] */ LIBRARYSAVEFLAGS lsf,
        //    /* [out] */ __RPC__deref_out_opt IShellItem **ppsiSavedTo) = 0;
    }
}
