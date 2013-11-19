#pragma once
#include "..\include\NsExt.CShellViewImpl.h"


// QueryStatus() is called by the browser to determine which standard commands our extension supports.
STDMETHODIMP CShellViewImpl::QueryStatus ( const GUID* pguidCmdGroup, ULONG cCmds,
										  OLECMD prgCmds[], OLECMDTEXT* pCmdText )
{
	if ( NULL == prgCmds )
		return E_POINTER;

	for ( UINT u = 0; u < cCmds; u++ )
	{
		PrintLog(">> Query - DEFAULT: %u\n", prgCmds[u]);

		switch ( prgCmds[u].cmdID )
		{
		case OLECMDID_OPEN:
		case OLECMDID_PROPERTIES:
		case OLECMDID_CUT:
		case OLECMDID_COPY:
		case OLECMDID_PASTE:
		case OLECMDID_SELECTALL:
		case OLECMDID_CLEARSELECTION:
		case OLECMDID_UPDATECOMMANDS:
		case OLECMDID_REFRESH:
		case OLECMDID_STOP:
		case OLECMDID_HIDETOOLBARS:
		case OLECMDID_SETPROGRESSMAX:
		case OLECMDID_SETPROGRESSPOS:
		case OLECMDID_SETPROGRESSTEXT:
		case OLECMDID_SETTITLE:
		case OLECMDID_ONTOOLBARACTIVATED:
		case OLECMDID_FIND:
		case OLECMDID_DELETE:
		case OLECMDID_ENABLE_INTERACTION:
		case OLECMDID_ONUNLOAD:
		case OLECMDID_PREREFRESH:
		case OLECMDID_SHOWFIND:
		case OLECMDID_CLOSE:
		case OLECMDID_ALLOWUILESSSAVEAS:
		case OLECMDID_WINDOWSTATECHANGED:
			prgCmds[u].cmdf = OLECMDF_SUPPORTED | OLECMDF_ENABLED;
			break;
		}
	}

	return S_OK;
}


// Exec() is called when the user executes a command in Explorer that we have to deal with.
// http://msdn.microsoft.com/en-us/library/windows/desktop/ms691264(v=vs.85).aspx
STDMETHODIMP CShellViewImpl::Exec ( const GUID* pguidCmdGroup, DWORD nCmdID,
								   DWORD nCmdExecOpt, VARIANTARG* pvaIn,
								   VARIANTARG* pvaOut )
{
	HRESULT hrRet = OLECMDERR_E_UNKNOWNGROUP;

/*
* pguidCmdGroup							 GUID				nCmdID	CmdName
* -------------------------------------------------------------------------
* NULL														23		OLECMDID_STOP
* {000214D0-0000-0000-C000-000000000046} CGID_Explorer		39		OLECMDID_PREREFRESH
* {000214D0-0000-0000-C000-000000000046} CGID_Explorer		41		OLECMDID_SHOWMESSAGE
* {4AF07F10-D231-11D0-B942-00A0C90312E1} CGID_DefView		2		OLECMDID_NEW
* {000214D1-0000-0000-C000-000000000046} CGID_ShellDocView	104
* {000214D1-0000-0000-C000-000000000046} CGID_ShellDocView	18		OLECMDID_CLEARSELECTION
*/
	// The only standard command we act on is "refresh".

	if ( NULL == pguidCmdGroup )
	{
		PrintLog("Exec - DEFAULT: %u", nCmdID);

		if ( OLECMDID_REFRESH == nCmdID )
		{
			Refresh();
			hrRet = S_OK;
		}
	}
	else if ( CGID_Explorer == *pguidCmdGroup )
	{
		PrintLog("Exec - EXPLORER : %u", nCmdID);

		if ( OLECMDID_PREREFRESH == nCmdID )
		{
			hrRet = S_OK;
		}

	}
	else if ( CGID_ShellDocView == *pguidCmdGroup )
	{
		PrintLog("Exec - DOCVIEW: %u", nCmdID);
	}

	return hrRet;
}
