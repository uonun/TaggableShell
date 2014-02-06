#pragma once
#include "..\include\Form.TagManager.h"

LRESULT CALLBACK Callback_SetTagByRecordId()
{
	FormTagManager *pebhd = pebhd = FormTagManager::instance();
	pebhd->LoadTags();
	return 0;
}

LRESULT CALLBACK DlgProc_TagManager(_In_  HWND hwnd,_In_  UINT uMsg,_In_  WPARAM wParam,_In_  LPARAM lParam){
	FormTagManager *pebhd = reinterpret_cast<FormTagManager *>(GetWindowLongPtr(hwnd, DWLP_USER));
	if (uMsg == WM_INITDIALOG)
	{
		pebhd = FormTagManager::instance();
		SetWindowLongPtr(hwnd, DWLP_USER, reinterpret_cast<LONG_PTR>(pebhd));
	}
	return pebhd ? pebhd->DlgProc(hwnd, uMsg, wParam, lParam) : 0;
}

LRESULT CALLBACK DlgProc_About(_In_  HWND hwnd,_In_  UINT uMsg,_In_  WPARAM wParam,_In_  LPARAM lParam){
	switch(uMsg)     
	{   
	case WM_INITDIALOG:
		{
			SetWindowText(hwnd,::MyLoadString(IDS_DLG_ABOUT_BU_CLOSE));
			SetDlgItemText(hwnd, IDC_ABOUT_BU_CLOSE,::MyLoadString(IDS_DLG_ABOUT_BU_CLOSE));

			wchar_t info[LOADSTRING_BUFFERSIZE] = {0};
			StrCat(info,::MyLoadString(IDS_ProductName));StrCat(info,L"\r\n");
			StrCat(info,::MyLoadString(IDS_ProductIntro));StrCat(info,L"\r\n");
			StrCat(info,L"Author: ");
			StrCat(info,::MyLoadString(IDS_ProductAuthor));StrCat(info,L"\r\n");
			StrCat(info,L"Contact: ");
			StrCat(info,::MyLoadString(IDS_ProductAuthorMail));StrCat(info,L"\r\n");
			StrCat(info,L"Homepage: ");
			StrCat(info,::MyLoadString(IDS_ProductHomepage));StrCat(info,L"\r\n");
			SetDlgItemText(hwnd, IDC_ABOUT_INFO,info);

			SendDlgItemMessage(hwnd, IDC_ABOUT_ICON, STM_SETIMAGE,IMAGE_ICON,(WPARAM)LoadIcon(g_hInst, MAKEINTRESOURCE(IDI_ICON)));

			break;
		}
	case WM_COMMAND:
		{
			switch(LOWORD(wParam))     
			{    
			case IDC_ABOUT_BU_CLOSE:
				{
					AnimateWindow(hwnd,200, AW_HIDE | AW_VER_NEGATIVE);
					DestroyWindow(hwnd);
				}
				break;
			default:
				break;
			}
		}
		break;
	case WM_SYSCOMMAND:
		{
			switch (wParam)
			{
			case SC_CLOSE:
				{
					AnimateWindow(hwnd,200, AW_HIDE | AW_VER_NEGATIVE);
					DestroyWindow(hwnd);
				}
				break;
			default:
				break;
			}
			break;
		}
	case WM_DESTROY:     
		{
			PostQuitMessage(0);
		}
		break;
	default:
		break;
	}

	return (INT_PTR)FALSE;
}

LRESULT CALLBACK DlgProc_Settings(_In_  HWND hwnd,_In_  UINT uMsg,_In_  WPARAM wParam,_In_  LPARAM lParam){
	switch(uMsg)     
	{   
	case WM_INITDIALOG:
		{
			SetWindowText(hwnd,::MyLoadString(IDS_DLG_ABOUT_CAPTION));
			break;
		}
	case WM_COMMAND:
		{
			switch(LOWORD(wParam))     
			{    
			case IDC_ABOUT_BU_CLOSE:
				DestroyWindow(hwnd);
				break;
			default:
				break;
			}
		}
		break;
	case WM_SYSCOMMAND:
		{
			switch (wParam)
			{
			case SC_CLOSE:
				DestroyWindow(hwnd);
				break;
			default:
				break;
			}
			break;
		}
	case WM_DESTROY:     
		{
			PostQuitMessage(0);
		}
		break;
	default:
		break;
	}

	return (INT_PTR)FALSE;
}
