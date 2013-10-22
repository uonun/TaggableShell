#pragma once
#include "dllmain.h"

class UnknownImpl
{
public:
	UnknownImpl(void);
	virtual ~UnknownImpl(void);

	// IUnknown
	IFACEMETHODIMP_(ULONG) AddRef();
	IFACEMETHODIMP_(ULONG) Release();

private:
	// IUnknown
	long _cRef;

};

