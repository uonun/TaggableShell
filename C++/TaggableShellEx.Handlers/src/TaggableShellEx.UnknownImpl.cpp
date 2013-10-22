#include "..\include\TaggableShellEx.UnknownImpl.h"


UnknownImpl::UnknownImpl(void)
{
}


UnknownImpl::~UnknownImpl(void)
{
}


IFACEMETHODIMP_(ULONG) UnknownImpl::AddRef()
{
	return InterlockedIncrement(&_cRef);
}

IFACEMETHODIMP_(ULONG) UnknownImpl::Release()
{
	long cRef = InterlockedDecrement(&_cRef);
	if (cRef == 0)
	{
		delete this;
	}
	return cRef;
}
