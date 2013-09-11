#pragma once
#include "ClassFactory.h"

HRESULT CClassFactory::CreateInstance(REFCLSID clsid, const CLASS_OBJECT_INIT *pClassObjectInits, size_t cClassObjectInits, REFIID riid, void **ppv)
{
	*ppv = NULL;
	HRESULT hr = CLASS_E_CLASSNOTAVAILABLE;
	for (size_t i = 0; i < cClassObjectInits; i++)
	{
		if (clsid == *pClassObjectInits[i].pClsid)
		{
			IClassFactory *pClassFactory = new (std::nothrow) CClassFactory(pClassObjectInits[i].pfnCreate);
			hr = pClassFactory ? S_OK : E_OUTOFMEMORY;
			if (SUCCEEDED(hr))
			{
				hr = pClassFactory->QueryInterface(riid, ppv);
				pClassFactory->Release();
			}
			break; // match found
		}
	}
	return hr;
}

CClassFactory::CClassFactory(PFNCREATEINSTANCE pfnCreate) : _cRef(1), _pfnCreate(pfnCreate)
{
	DllAddRef();
}

// IUnknown
IFACEMETHODIMP CClassFactory::QueryInterface(REFIID riid, void ** ppv)
{
	static const QITAB qit[] =
	{
		QITABENT(CClassFactory, IClassFactory),
		{ 0 }
	};
	return QISearch(this, qit, riid, ppv);
}

IFACEMETHODIMP_(ULONG) CClassFactory::AddRef()
{
	return InterlockedIncrement(&_cRef);
}

IFACEMETHODIMP_(ULONG) CClassFactory::Release()
{
	long cRef = InterlockedDecrement(&_cRef);
	if (cRef == 0)
	{
		delete this;
	}
	return cRef;
}


// IClassFactory
IFACEMETHODIMP CClassFactory::CreateInstance(IUnknown *punkOuter, REFIID riid, void **ppv)
{
	return punkOuter ? CLASS_E_NOAGGREGATION : _pfnCreate(riid, ppv);
}

IFACEMETHODIMP CClassFactory::LockServer(BOOL fLock)
{
	if (fLock)
	{
		DllAddRef();
	}
	else
	{
		DllRelease();
	}
	return S_OK;
}

CClassFactory::~CClassFactory()
{
	DllRelease();
}
