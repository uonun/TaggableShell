#pragma once
#include <windows.h>
#include <shlwapi.h>
#include <shlobj.h>
#include "TagHandler.h"

TagHandler::TagHandler() : _cRef(1)
{
	DllAddRef();
}

TagHandler::~TagHandler(void){   
	DllRelease();
}

HRESULT TagHandler_CreateInstance(REFIID riid, void **ppv)
{
	HRESULT hr = E_OUTOFMEMORY;
	TagHandler *pirm = new (std::nothrow) TagHandler();
	if (pirm)
	{
		hr = pirm->QueryInterface(riid, ppv);
		pirm->Release();
	}
	return hr;
}

// IUnknown
IFACEMETHODIMP TagHandler::QueryInterface(REFIID riid, void ** ppv)
{
	static const QITAB qit[] =
	{
		QITABENT(TagHandler, IInitializeWithStream),
		{0 },
	};
	return QISearch(this, qit, riid, ppv);
}

IFACEMETHODIMP_(ULONG) TagHandler::AddRef()
{
	return InterlockedIncrement(&_cRef);
}

IFACEMETHODIMP_(ULONG) TagHandler::Release()
{
	long cRef = InterlockedDecrement(&_cRef);
	if (cRef == 0)
	{
		delete this;
	}
	return cRef;
}

HRESULT TagHandler::Initialize(IStream *pStream, DWORD grfMode)
{
	HRESULT hr = E_UNEXPECTED;
	if (!_pStream)
	{
		// save a reference to the stream as well as the grfMode
		hr = pStream->QueryInterface(&_pStream);
		if (SUCCEEDED(hr))
		{
			_grfMode = grfMode;
		}
	}
	return hr;
}

