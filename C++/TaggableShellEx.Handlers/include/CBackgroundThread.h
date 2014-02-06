#pragma once

template<class T1,typename TArgs>
class CBackgroundThread
{
public:
	CBackgroundThread(T1 *pebhd);
	// there must be a method T1::DoWorkAsyn(TArgs*) to invoke.
	HRESULT StartThread(TArgs* & prf);

	ULONG AddRef();
	ULONG Release();


private:
	static DWORD WINAPI s_ThreadProc(void *pv);

	~CBackgroundThread();

	long _cRef;
	T1 *_pebhd;
	IStream *_pStream;
};

////////////////////////////////////////////////////////////////////////////////////
// AS TEMPLATE CLASS, THE IMPLEMENTATION CHOULD NOT BE SEPERATED WITH DEFINATION. //
////////////////////////////////////////////////////////////////////////////////////
template<class T1,typename TArgs>
CBackgroundThread<T1,TArgs>::CBackgroundThread(T1 *pebhd) : _cRef(1), _pebhd(pebhd), _pStream(NULL)
{
	_pebhd->AddRef();
}

template<class T1,typename TArgs>
HRESULT CBackgroundThread<T1,TArgs>::StartThread(TArgs* & prf)
{
	HRESULT hr = CoMarshalInterThreadInterfaceInStream(__uuidof(prf), prf, &_pStream);
	if (SUCCEEDED(hr))
	{
		AddRef();
		hr = SHCreateThread(s_ThreadProc, this, CTF_COINIT, NULL) ? S_OK : E_FAIL;
		if (FAILED(hr))
		{
			Release();
		}
	}
	return hr;
}


template<class T1,typename TArgs>
ULONG CBackgroundThread<T1,TArgs>::AddRef()
{
	return InterlockedIncrement(&_cRef);
}

template<class T1,typename TArgs>
ULONG CBackgroundThread<T1,TArgs>::Release()
{
	long cRef = InterlockedDecrement(&_cRef);
	if (!cRef)
		delete this;
	return cRef;
}

template<class T1,typename TArgs>
DWORD WINAPI CBackgroundThread<T1,TArgs>::s_ThreadProc(void *pv)
{
	CBackgroundThread<T1,TArgs> *pfrobt = reinterpret_cast<CBackgroundThread<T1,TArgs>*>(pv);

	TArgs *prf;
	HRESULT hr = S_OK;
	if ( NULL != pfrobt->_pStream )
	{
		hr = CoGetInterfaceAndReleaseStream(pfrobt->_pStream, IID_PPV_ARGS(&prf));
		prf->AddRef();
		pfrobt->_pStream = NULL;
	}

	if (SUCCEEDED(hr))
	{
		// there must be a method T1::DoWorkAsyn(TArgs).
		pfrobt->_pebhd->DoWorkAsyn(prf);
		prf->Release();
	}

	// The callee transfered the reference to s_ThreadProc via SHCreateThread
	pfrobt->Release();
	return 0;
}

template<class T1,typename TArgs>
CBackgroundThread<T1,TArgs>::~CBackgroundThread()
{
	_pebhd->Release();
	if (_pStream)
	{
		_pStream->Release();
	}
}
