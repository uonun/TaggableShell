#pragma once
#include "StdAfx.h"
#include "TaggableShellEx.Utils.h"

class DECLSPEC_UUID("20130929-1982-0805-819F-6D66853DDD60") CHandler;
class DECLSPEC_UUID("20130929-1982-0805-819F-6D66853DDD70") CShellFolderImpl;
class DECLSPEC_UUID("20130929-1982-0805-819F-6D66853DDD71") CShellViewImpl;
HRESULT CHandler_CreateInstance(REFIID riid, void **ppv);
HRESULT CShellFolderImpl_CreateInstance(REFIID riid, void **ppv);
HRESULT CShellViewImpl_CreateInstance(REFIID riid, void **ppv);

HRESULT RegisterHandler();
HRESULT UnregisterHandler();

void DllAddRef();
void DllRelease();
