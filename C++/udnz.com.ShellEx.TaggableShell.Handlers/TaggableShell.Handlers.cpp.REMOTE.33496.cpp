
#include <ShObjIdl.h>
#include <ShlObj.h>
#include "stdafx.h"

class Handlers: public IShellExtInit
{
public:
	LPCITEMIDLIST  m_pIDFolder;           //The folder's PIDL
	TCHAR          m_szFile[MAX_PATH];    //The file name
	IDataObject   *m_pDataObj;            //The IDataObject pointer
	HKEY           m_hRegKey;             //The file or folder's registry key

	STDMETHODIMP Initialize(LPCITEMIDLIST pIDFolder, 
		IDataObject *pDataObj, 
		HKEY hRegKey) 
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
		{ 
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
			RegOpenKeyEx(hRegKey, 
			NULL, 
			0L, 
			MAXIMUM_ALLOWED, 
			&m_hRegKey); 
		return S_OK; 
	}
}
}