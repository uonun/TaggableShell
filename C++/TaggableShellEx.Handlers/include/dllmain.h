#pragma once
#include "StdAfx.h"
#include "Utils.h"

class DECLSPEC_UUID(CLSID_CHandler) CHandler;
class DECLSPEC_UUID(CLSID_CShellFolderImpl) CShellFolderImpl;
class DECLSPEC_UUID(CLSID_CShellViewImpl) CShellViewImpl;

HRESULT CHandler_CreateInstance(REFIID riid, void **ppv);
HRESULT CShellFolderImpl_CreateInstance(REFIID riid, void **ppv);
HRESULT CShellViewImpl_CreateInstance(REFIID riid, void **ppv);

HRESULT RegisterHandler();
HRESULT UnregisterHandler();

void DllAddRef();
void DllRelease();

