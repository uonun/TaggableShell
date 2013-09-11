#pragma once
#include "dllmain.h"

class CClassFactory : public IClassFactory
{
public:
    CClassFactory(PFNCREATEINSTANCE pfnCreate);

    static HRESULT CreateInstance(REFCLSID clsid, const CLASS_OBJECT_INIT *pClassObjectInits, size_t cClassObjectInits, REFIID riid, void **ppv);

    // IUnknown
    IFACEMETHODIMP QueryInterface(REFIID riid, void ** ppv);

    IFACEMETHODIMP_(ULONG) AddRef();

    IFACEMETHODIMP_(ULONG) Release();


    // IClassFactory
    IFACEMETHODIMP CreateInstance(IUnknown *punkOuter, REFIID riid, void **ppv);

    IFACEMETHODIMP LockServer(BOOL fLock);

private:
    ~CClassFactory();

    long _cRef;
    PFNCREATEINSTANCE _pfnCreate;
};
