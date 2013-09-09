<<<<<<< HEAD
#pragma once
#include <ShObjIdl.h>
#include <ShlObj.h>
#include "stdafx.h"
using namespace std;


class Handlers{
private:
=======

#include <ShObjIdl.h>
#include <ShlObj.h>
#include "stdafx.h"

class Handlers: public IShellExtInit
{
public:
>>>>>>> 71dc0202a4c0ac28a7357deb9ed28a437eaf9360
	LPCITEMIDLIST  m_pIDFolder;           //The folder's PIDL
	TCHAR          m_szFile[MAX_PATH];    //The file name
	IDataObject   *m_pDataObj;            //The IDataObject pointer
	HKEY           m_hRegKey;             //The file or folder's registry key

<<<<<<< HEAD
public:
	Handlers(void){}
	~Handlers(void){}

	STDMETHODIMP Initialize(LPCITEMIDLIST pIDFolder,IDataObject *pDataObj,HKEY hRegKey)
=======
	STDMETHODIMP Initialize(LPCITEMIDLIST pIDFolder, 
		IDataObject *pDataObj, 
		HKEY hRegKey) 
>>>>>>> 71dc0202a4c0ac28a7357deb9ed28a437eaf9360
	{ 
		// If Initialize has already been called, release the old PIDL
		ILFree((LPITEMIDLIST)m_pIDFolder);
		m_pIDFolder = NULL;

		//Store the new PIDL.
		if(pIDFolder)
		{
			m_pIDFolder = ILClone(pIDFolder);
		}

		// If Initialize has already been called, release the old
		// IDataObject pointer.
		if (m_pDataObj)
<<<<<<< HEAD
		{
=======
		{ 
>>>>>>> 71dc0202a4c0ac28a7357deb9ed28a437eaf9360
			m_pDataObj->Release(); 
		}

		// If a data object pointer was passed in, save it and
		// extract the file name. 
		if (pDataObj) 
		{ 
			m_pDataObj = pDataObj; 
			pDataObj->AddRef(); 

			STGMEDIUM   medium;
			FORMATETC   fe = {CF_HDROP, NULL, DVASPECT_CONTENT, -1, TYMED_HGLOBAL};
			UINT        uCount;

			if(SUCCEEDED(m_pDataObj->GetData(&fe, &medium)))
			{
				// Get the count of files dropped.
				uCount = DragQueryFile((HDROP)medium.hGlobal, (UINT)-1, NULL, 0);

				// Get the first file name from the CF_HDROP.
				if(uCount)
					DragQueryFile((HDROP)medium.hGlobal, 0, m_szFile, 
					sizeof(m_szFile)/sizeof(TCHAR));

				ReleaseStgMedium(&medium);
			}
		}

		// Duplicate the registry handle. 
		if (hRegKey) 
<<<<<<< HEAD
			RegOpenKeyEx(hRegKey,NULL,0L,MAXIMUM_ALLOWED,&m_hRegKey); 

		return S_OK; 
	}
};
=======
			RegOpenKeyEx(hRegKey, 
			NULL, 
			0L, 
			MAXIMUM_ALLOWED, 
			&m_hRegKey); 
		return S_OK; 
	}
}
}
>>>>>>> 71dc0202a4c0ac28a7357deb9ed28a437eaf9360
