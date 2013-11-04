#pragma once
#include "..\include\NsExt.CShellViewImpl.h"


// QueryStatus() is called by the browser to determine which standard commands
// our extension supports.
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
	/*
	// http://msdn.microsoft.com/en-us/library/windows/desktop/ms691264(v=vs.85).aspx
	if ( NULL == pguidCmdGroup )
	{
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
	else if ( CGID_Explorer == *pguidCmdGroup )
	{
		for ( UINT u = 0; u < cCmds; u++ )
		{
			PrintLog(">> Query - EXPLORER: %u\n", prgCmds[u]);
		}
	}
	else if ( CGID_ShellDocView == *pguidCmdGroup )
	{
		for ( UINT u = 0; u < cCmds; u++ )
		{
			PrintLog(">> Query - DOCVIEW: %u\n", prgCmds[u]);
		}
	}*/

	return OLECMDERR_E_UNKNOWNGROUP;
}


// Exec() is called when the user executes a command in Explorer that we
// have to deal with.
STDMETHODIMP CShellViewImpl::Exec ( const GUID* pguidCmdGroup, DWORD nCmdID,
								   DWORD nCmdExecOpt, VARIANTARG* pvaIn,
								   VARIANTARG* pvaOut )
{
	HRESULT hrRet = OLECMDERR_E_UNKNOWNGROUP;

	// The only standard command we act on is "refresh".  I've put
	// some trace messages in so you can see the other commands that the
	// browser sends our way.  If you can figure out what they're all for,
	// let me know!

	if ( NULL == pguidCmdGroup )
	{
		PrintLog(">> Exec - DEFAULT: %u\n", nCmdID);

		if ( OLECMDID_REFRESH == nCmdID )
		{
			Refresh();
			hrRet = S_OK;
		}
	}
	else if ( CGID_Explorer == *pguidCmdGroup )
	{
		PrintLog(">> Exec - EXPLORER : %u\n", nCmdID);
	}
	else if ( CGID_ShellDocView == *pguidCmdGroup )
	{
		PrintLog(">> Exec - DOCVIEW: %u\n", nCmdID);
	}

	return hrRet;
}
