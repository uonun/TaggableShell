#pragma once
#include "../include/dllmain.h"
#include "../include/TaggableShellEx.Taghelper.h"
#include "../include/TaggableShellEx.CHandler.h"
#include <propsys.h>     // Property System APIs and interfaces
#include <propkey.h>     // System PROPERTYKEY definitions


// {1794C9FE-74A9-497f-9C69-B31F03CE7EF9} 100
const PROPERTYKEY PKEY_Microsoft_SampleRecipe_Difficulty = {{0x1794c9fe, 0x74a9, 0x497f, 0x9c, 0x69, 0xb3, 0x1f, 0x3, 0xce, 0x7e, 0xf9}, 100};

const PROPERTYMAP g_rgPROPERTYMAP[] =
{
    { &PKEY_Title,                             L"Recipe",                L"Title" },
    { &PKEY_Comment,                           L"Recipe",                L"Comments" },
    { &PKEY_Author,                            L"Recipe/Background",     L"Author" },
    { &PKEY_Keywords,                          L"Recipe/RecipeKeywords", L"Keyword" },
    { &PKEY_Microsoft_SampleRecipe_Difficulty, L"Recipe/RecipeInfo",     L"Difficulty" },
};

// Accessor methods forward directly to internal value cache
HRESULT CHandler::GetCount(DWORD *pcProps)
{
    *pcProps = 0;
    return _pCache ? _pCache->GetCount(pcProps) : E_UNEXPECTED;
}

HRESULT CHandler::GetAt(DWORD iProp, PROPERTYKEY *pkey)
{
    *pkey = PKEY_Null;
    return _pCache ? _pCache->GetAt(iProp, pkey) : E_UNEXPECTED;
}

HRESULT CHandler::GetValue(REFPROPERTYKEY key, PROPVARIANT *pPropVar)
{
    PropVariantInit(pPropVar);
    return _pCache ? _pCache->GetValue(key, pPropVar) : E_UNEXPECTED;
}

// SetValue just updates the internal value cache
HRESULT CHandler::SetValue(REFPROPERTYKEY key, REFPROPVARIANT propVar)
{
    HRESULT hr = E_UNEXPECTED;
    if (_pCache)
    {
        // check grfMode to ensure writes are allowed
        hr = STG_E_ACCESSDENIED;
        if ((_grfMode & STGM_READWRITE) &&
            (key != PKEY_Search_Contents))  // this property is read-only
        {
            hr = _pCache->SetValueAndState(key, &propVar, PSC_DIRTY);
        }
    }

    return hr;
}

// Commit writes the internal value cache back out to the stream passed to Initialize
HRESULT CHandler::Commit()
{
    HRESULT hr = E_UNEXPECTED;
    if (_pCache)
    {
		/*
        // check grfMode to ensure writes are allowed
        hr = STG_E_ACCESSDENIED;
        if (_grfMode & STGM_READWRITE)
        {
            // save the internal value cache to XML DOM object
            hr = _SaveCacheToDom();
            if (SUCCEEDED(hr))
            {
                // reset the output stream
                LARGE_INTEGER liZero = {};
                hr = _pStream->Seek(liZero, STREAM_SEEK_SET, NULL);
                if (SUCCEEDED(hr))
                {
                    // obtain a temporary destination stream for manual safe-save
                    IDestinationStreamFactory *pSafeCommit;
                    hr = _pStream->QueryInterface(&pSafeCommit);
                    if (SUCCEEDED(hr))
                    {
                        IStream *pStreamCommit;
                        hr = pSafeCommit->GetDestinationStream(&pStreamCommit);
                        if (SUCCEEDED(hr))
                        {
                            // write the XML out to the temprorary stream and commit it
                            VARIANT varStream = {};
                            varStream.vt = VT_UNKNOWN;
                            varStream.punkVal = pStreamCommit;
                            hr = _pDomDoc->save(varStream);
                            if (SUCCEEDED(hr))
                            {
                                hr = pStreamCommit->Commit(STGC_DEFAULT);
                                if (SUCCEEDED(hr))
                                {
                                    // commit the real output stream
                                    _pStream->Commit(STGC_DEFAULT);
                                }
                            }

                            pStreamCommit->Release();
                        }

                        pSafeCommit->Release();
                    }
                }
            }
        }*/
    }

    return hr;
}

// Indicates whether the users should be able to edit values for the given property key
HRESULT CHandler::IsPropertyWritable(REFPROPERTYKEY key)
{
    // System.Search.Contents is the only property not supported for writing
    return (key == PKEY_Search_Contents) ? S_FALSE : S_OK;
}





/*


// Populates the internal value cache from the internal DOM object
HRESULT CHandler::_LoadCacheFromDom()
{
    HRESULT hr = S_OK;

    if (!_pCache)
    {
        // create the internal value cache
        hr = PSCreateMemoryPropertyStore(IID_PPV_ARGS(&_pCache));
        if (SUCCEEDED(hr))
        {
            // populate native properties directly from the XML
            for (UINT i = 0; i < ARRAYSIZE(g_rgPROPERTYMAP); ++i)
            {
                _LoadProperty(g_rgPROPERTYMAP[i]);
            }

            // load extended properties and search content
            _LoadExtendedProperties();
            _LoadSearchContent();
        }
    }

    return hr;
}

// Loads specified values from given parent node and creates a PROPVARIANT from them
HRESULT CHandler::_LoadPropertyValues(IXMLDOMNode *pNodeParent,
                                                    PCWSTR pszValueNodeName,
                                                    PROPVARIANT *ppropvarValues)
{
    // intialize the outparam
    PropVariantInit(ppropvarValues);

    // select the value nodes
    IXMLDOMNodeList *pValueList = NULL;
    HRESULT hr = pNodeParent->selectNodes(CastToBSTRForInput(pszValueNodeName), &pValueList);
    if (hr == S_OK)
    {
        // get the count of values
        long cValues = 0;
        hr = pValueList->get_length(&cValues);
        if (SUCCEEDED(hr))
        {
            // create an array to hold the values
            BSTR *pbstrValues = new(std::nothrow) BSTR[cValues];
            hr = pbstrValues ? S_OK : E_OUTOFMEMORY;
            if (SUCCEEDED(hr))
            {
                // load the text of each value node into the array
                for (long iValue = 0; iValue < cValues; iValue++)
                {
                    if (hr == S_OK)
                    {
                        IXMLDOMNode *pValue = NULL;
                        hr = pValueList->get_item(iValue, &pValue);
                        if (hr == S_OK)
                        {
                            hr = pValue->get_text(&pbstrValues[iValue]);
                            pValue->Release();
                        }
                    }
                    else
                    {
                        // NULL out remaining elements on failure
                        pbstrValues[iValue] = NULL;
                    }
                }

                if (hr == S_OK)
                {
                    // package the list of values up in a PROPVARIANT
                    hr = InitPropVariantFromStringVector(const_cast<PCWSTR*>(pbstrValues), cValues, ppropvarValues);
                }

                // clean up array of values
                for (long iValue = 0; iValue < cValues; iValue++)
                {
                    SysFreeString(pbstrValues[iValue]);
                }
                delete[] pbstrValues;
            }
        }
    }

    return hr;
}

// Loads the data for the property specified in the given map into the internal value cache
HRESULT CHandler::_LoadProperty(const PROPERTYMAP &map)
{
    // select the property's parent node and load the property's value(s)
    IXMLDOMNode *pNodeParent = NULL;
    HRESULT hr = _pDomDoc->selectSingleNode(CastToBSTRForInput(map.pszXPathParent), &pNodeParent);
    if (hr == S_OK)
    {
        PROPVARIANT propvarValues = {};
        hr = _LoadPropertyValues(pNodeParent, map.pszValueNodeName, &propvarValues);
        if (hr == S_OK)
        {
            // coerce the value(s) to the appropriate type for the property key
            hr = PSCoerceToCanonicalValue(*map.pkey, &propvarValues);
            if (SUCCEEDED(hr))
            {
                // cache the value(s) loaded
                hr = _pCache->SetValueAndState(*map.pkey, &propvarValues, PSC_NORMAL);
            }
            PropVariantClear(&propvarValues);
        }
        pNodeParent->Release();
    }

    return hr;
}

// Loads data for any external properties (e.g. those not explicitly mapped to the XML schema) into the internal value cache
HRESULT CHandler::_LoadExtendedProperties()
{
    // select the list of extended property nodes
    IXMLDOMNodeList *pList = NULL;
    HRESULT hr = _pDomDoc->selectNodes(CastToBSTRForInput(L"Recipe/ExtendedProperties/Property"), &pList);
    if (hr == S_OK)
    {
        long cElems = 0;
        hr = pList->get_length(&cElems);
        if (hr == S_OK)
        {
            // iterate over the list and cache each value
            for (long iElem = 0; iElem < cElems; ++iElem)
            {
                IXMLDOMNode *pNode = NULL;
                hr = pList->get_item(iElem, &pNode);
                if (hr == S_OK)
                {
                    IXMLDOMElement *pElement = NULL;
                    hr = pNode->QueryInterface(IID_PPV_ARGS(&pElement));
                    if (SUCCEEDED(hr))
                    {
                        // get the name of the property and convert it to a PROPERTYKEY
                        VARIANT varPropKey = {};
                        hr = pElement->getAttribute(CastToBSTRForInput(L"Key"), &varPropKey);
                        if (hr == S_OK)
                        {
                            PROPERTYKEY key;
                            hr = PSPropertyKeyFromString(varPropKey.bstrVal, &key);
                            if (SUCCEEDED(hr))
                            {
                                // get the encoded value and deserialize it into a PROPVARIANT
                                VARIANT varEncodedValue = {};
                                hr = pElement->getAttribute(CastToBSTRForInput(L"EncodedValue"), &varEncodedValue);
                                if (hr == S_OK)
                                {
                                    PROPVARIANT propvarValue = {};
                                    hr = DeserializePropVariantFromString(varEncodedValue.bstrVal, &propvarValue);
                                    if (SUCCEEDED(hr))
                                    {
                                        // cache the value loaded
                                        hr = _pCache->SetValueAndState(key, &propvarValue, PSC_NORMAL);
                                        PropVariantClear(&propvarValue);
                                    }

                                    VariantClear(&varEncodedValue);
                                }
                            }

                            VariantClear(&varPropKey);
                        }

                        pElement->Release();
                    }
                    pNode->Release();
                }
            }
        }
        pList->Release();
    }

    return hr;
}

// Populates the System.Search.Contents property in the internal value cache
HRESULT CHandler::_LoadSearchContent()
{
    // XSLT to generate a space-delimited list of Items, Steps, Yield, Difficulty, and Keywords
    BSTR bstrContentXSLT = SysAllocString(L"<xsl:stylesheet version=\"1.0\" xmlns:xsl=\"http://www.w3.org/1999/XSL/Transform\">\n"
                                          L"<xsl:output method=\"text\" version=\"1.0\" encoding=\"UTF-8\" indent=\"no\"/>\n"
                                          L"  <xsl:template match=\"/\">\n"
                                          L"    <xsl:apply-templates select=\"Recipe/Ingredients/Item\"/>\n"
                                          L"    <xsl:apply-templates select=\"Recipe/Directions/Step\"/>\n"
                                          L"    <xsl:apply-templates select=\"Recipe/RecipeInfo/Yield\"/>\n"
                                          L"    <xsl:apply-templates select=\"Recipe/RecipeInfo/Difficulty\"/>\n"
                                          L"    <xsl:apply-templates select=\"Recipe/RecipeKeywords/Keyword\"/>\n"
                                          L"  </xsl:template>\n"
                                          L"  <xsl:template match=\"*\">\n"
                                          L"    <xsl:value-of select=\".\"/>\n"
                                          L"    <xsl:text> </xsl:text>\n"
                                          L"  </xsl:template>\n"
                                          L"</xsl:stylesheet>");
    HRESULT hr = bstrContentXSLT ? S_OK : E_OUTOFMEMORY;
    if (SUCCEEDED(hr))
    {
        // create a DOM object to hold the XSLT
        IXMLDOMDocument *pContentXSLT = NULL;
        hr = CoCreateInstance(__uuidof(DOMDocument60), NULL, CLSCTX_INPROC_SERVER,
                              IID_PPV_ARGS(&pContentXSLT));
        if (SUCCEEDED(hr))
        {
            // load the XSLT
            VARIANT_BOOL vfSuccess = VARIANT_FALSE;
            hr = pContentXSLT->loadXML(bstrContentXSLT, &vfSuccess);
            if (!vfSuccess)
            {
                hr = FAILED(hr) ? hr : E_FAIL; // keep failed hr
            }

            if (SUCCEEDED(hr))
            {
                // get the root node of the XSLT
                IXMLDOMNode* pContentXSLTNode = NULL;
                hr = pContentXSLT->QueryInterface(IID_PPV_ARGS(&pContentXSLTNode));
                if (SUCCEEDED(hr))
                {
                    // transform the internal DOM object using the XSLT to generate the content string
                    BSTR bstrContent = NULL;
                    hr = _pDomDoc->transformNode(pContentXSLTNode, &bstrContent);
                    if (SUCCEEDED(hr))
                    {
                        // initialize a PROPVARIANT from the string, and store it in the internal value cache
                        PROPVARIANT propvarContent = {};
                        hr = InitPropVariantFromString(bstrContent, &propvarContent);
                        if (SUCCEEDED(hr))
                        {
                            hr = _pCache->SetValueAndState(PKEY_Search_Contents, &propvarContent, PSC_NORMAL);
                            PropVariantClear(&propvarContent);
                        }

                        SysFreeString(bstrContent);
                    }
                    pContentXSLT->Release();
                }
            }
            pContentXSLT->Release();
        }
        SysFreeString(bstrContentXSLT);
    }

    return hr;
}

// Saves the values in the internal cache back to the internal DOM object
HRESULT CHandler::_SaveCacheToDom()
{
    // iterate over each property in the internal value cache
    DWORD cProps;
    HRESULT hr = _pCache->GetCount(&cProps);
    for (UINT i = 0; SUCCEEDED(hr) && (i < cProps); ++i)
    {
        PROPERTYKEY key;
        hr = _pCache->GetAt(i, &key);
        if (SUCCEEDED(hr))
        {
            // check the cache state; only save dirty properties
            PSC_STATE psc;
            hr = _pCache->GetState(key, &psc);
            if (SUCCEEDED(hr) && (psc == PSC_DIRTY))
            {
                // get the cached value
                PROPVARIANT propvar = {};
                hr = _pCache->GetValue(key, &propvar);
                if (SUCCEEDED(hr))
                {
                    // save as a native property if the key is in the property map
                    BOOL fIsNativeProperty = FALSE;
                    for (UINT i = 0; i < ARRAYSIZE(g_rgPROPERTYMAP); ++i)
                    {
                        if (key == *g_rgPROPERTYMAP[i].pkey)
                        {
                            fIsNativeProperty = TRUE;
                            hr = _SaveProperty(propvar, g_rgPROPERTYMAP[i]);
                            break;
                        }
                    }

                    // otherwise, save as an extended proeprty
                    if (!fIsNativeProperty)
                    {
                        hr = _SaveExtendedProperty(key, propvar);
                    }

                    PropVariantClear(&propvar);
                }
            }
        }
    }

    return hr;
}

// Saves the values in the given PROPVARIANT to the specified XML nodes
HRESULT CHandler::_SavePropertyValues(IXMLDOMNode* pNodeParent,
                                                    PCWSTR pszValueNodeName,
                                                    REFPROPVARIANT propvarValues)
{
    // iterate through each value in the PROPVARIANT
    HRESULT hr = S_OK;
    ULONG cValues = PropVariantGetElementCount(propvarValues);
    for (ULONG iValue = 0; SUCCEEDED(hr) && (iValue < cValues); iValue++)
    {
        PROPVARIANT propvarValue = {};
        hr = PropVariantGetElem(propvarValues, iValue, &propvarValue);
        if (SUCCEEDED(hr))
        {
            // convert to a BSTR
            BSTR bstrValue = NULL;
            hr = PropVariantToBSTR(propvarValue, &bstrValue);
            if (SUCCEEDED(hr))
            {
                // create an element and set its text to the value
                IXMLDOMElement *pValue = NULL;
                hr = _pDomDoc->createElement(CastToBSTRForInput(pszValueNodeName), &pValue);
                if (SUCCEEDED(hr))
                {
                    hr = pValue->put_text(bstrValue);
                    if (SUCCEEDED(hr))
                    {
                        // append the value to its parent node
                        hr = pNodeParent->appendChild(pValue, NULL);
                    }

                    pValue->Release();
                }
                SysFreeString(bstrValue);
            }
            PropVariantClear(&propvarValue);
        }
    }
    return hr;
}

// Saves the given PROPVARIANT value to the XML nodes specified by the specified map
HRESULT CHandler::_SaveProperty(REFPROPVARIANT propvar, const PROPERTYMAP &map)
{
    // obtain the parent node of the value
    IXMLDOMNode *pNodeParent = NULL;
    HRESULT hr = _pDomDoc->selectSingleNode(CastToBSTRForInput(map.pszXPathParent), &pNodeParent);
    if (hr == S_OK)
    {
        // remove existing value nodes
        IXMLDOMNodeList *pNodeListValues = NULL;
        hr = pNodeParent->selectNodes(CastToBSTRForInput(map.pszValueNodeName), &pNodeListValues);
        if (hr == S_OK)
        {
            IXMLDOMSelection *pSelectionValues = NULL;
            hr = pNodeListValues->QueryInterface(IID_PPV_ARGS(&pSelectionValues));
            if (SUCCEEDED(hr))
            {
                hr = pSelectionValues->removeAll();
                pSelectionValues->Release();
            }
            pNodeListValues->Release();
        }

        if (SUCCEEDED(hr))
        {
            // save the new values to the parent node
            hr = _SavePropertyValues(pNodeParent, map.pszValueNodeName, propvar);
        }

        pNodeParent->Release();
    }
    return hr;
}

// Saves an extended property with the given key and value
HRESULT CHandler::_SaveExtendedProperty(REFPROPERTYKEY key, REFPROPVARIANT propvar)
{
    // convert the key to string form; don't use canonical name, since it may not be registered on other systems
    WCHAR szKey[MAX_PATH] = {};
    HRESULT hr = PSStringFromPropertyKey(key, szKey, ARRAYSIZE(szKey));
    if (SUCCEEDED(hr))
    {
        // serialize the value to string form
        PWSTR pszValue;
        hr = SerializePropVariantAsString(propvar, &pszValue);
        if (SUCCEEDED(hr))
        {
            // obtain or create the ExtendedProperties node in the document
            IXMLDOMElement *pRecipe = NULL;
            hr = _pDomDoc->get_documentElement(&pRecipe);
            if (hr == S_OK)
            {
                IXMLDOMNode *pExtended = NULL;
                hr = _EnsureChildNodeExists(pRecipe, L"ExtendedProperties", NULL, &pExtended);
                if (SUCCEEDED(hr))
                {
                    // query for the Property node with the given key, or create a new Property node
                    WCHAR szPropertyPath[MAX_PATH];
                    hr = StringCchPrintfW(szPropertyPath, ARRAYSIZE(szPropertyPath), L"Property[@Key = '%s']", szKey);
                    if (SUCCEEDED(hr))
                    {
                        if (propvar.vt != VT_EMPTY)
                        {
                            IXMLDOMNode *pPropNode = NULL;
                            hr = _EnsureChildNodeExists(pExtended, L"Property", szPropertyPath, &pPropNode);
                            if (SUCCEEDED(hr))
                            {
                                IXMLDOMElement *pPropNodeElem = NULL;
                                hr = pPropNode->QueryInterface(IID_PPV_ARGS(&pPropNodeElem));
                                if (SUCCEEDED(hr))
                                {
                                    // set the attributes of the node with the name and value of the property
                                    VARIANT varKey = {};
                                    hr = InitVariantFromString(szKey, &varKey);
                                    if (SUCCEEDED(hr))
                                    {
                                        VARIANT varValue = {};
                                        hr = InitVariantFromString(pszValue, &varValue);
                                        if (SUCCEEDED(hr))
                                        {
                                            hr = pPropNodeElem->setAttribute(CastToBSTRForInput(L"Key"), varKey);
                                            if (SUCCEEDED(hr))
                                            {
                                                hr = pPropNodeElem->setAttribute(CastToBSTRForInput(L"EncodedValue"), varValue);
                                            }
                                            VariantClear(&varValue);
                                        }
                                        VariantClear(&varKey);
                                    }
                                    pPropNodeElem->Release();
                                }
                                pPropNode->Release();
                            }
                        }
                        else
                        {
                            // VT_EMPTY means "clear the value", so remove the corresponding node
                            IXMLDOMNode *pPropNode = NULL;
                            hr = pExtended->selectSingleNode(szPropertyPath, &pPropNode);
                            if (hr == S_OK)
                            {
                                IXMLDOMNode *pRemoved;
                                hr = pExtended->removeChild(pPropNode, &pRemoved);
                                if (SUCCEEDED(hr))
                                {
                                    pRemoved->Release();
                                }
                                pPropNode->Release();
                            }
                        }
                    }
                    pExtended->Release();
                }
                pRecipe->Release();
            }
            else
            {
                hr = E_UNEXPECTED;
            }

            CoTaskMemFree(pszValue);
        }
    }

    return hr;
}

// Queries for the specified child node, and creates and appends a new one if no such node exists
HRESULT CHandler::_EnsureChildNodeExists(IXMLDOMNode *pNodeParent, PCWSTR pszName, PCWSTR pszXPath, IXMLDOMNode **ppNodeChild)
{
    // query for the child node in case it already exists
    HRESULT hr = pNodeParent->selectSingleNode(CastToBSTRForInput(pszXPath ? pszXPath : pszName), ppNodeChild);
    if (hr != S_OK)
    {
        // create an element with the specified name and append it to the given parent node
        IXMLDOMElement *pChildElem = NULL;
        hr = _pDomDoc->createElement(CastToBSTRForInput(pszName), &pChildElem);
        if (SUCCEEDED(hr))
        {
            hr = pNodeParent->appendChild(pChildElem, ppNodeChild);
            pChildElem->Release();
        }
    }

    return hr;
}

// Serializes a PROPVARIANT value to string form
HRESULT SerializePropVariantAsString(REFPROPVARIANT propvar, PWSTR *ppszOut)
{
    SERIALIZEDPROPERTYVALUE *pBlob;
    ULONG cbBlob;

    // serialize PROPVARIANT to binary form
    HRESULT hr = StgSerializePropVariant(&propvar, &pBlob, &cbBlob);
    if (SUCCEEDED(hr))
    {
        // determine the required buffer size
        DWORD cchString;
        hr = CryptBinaryToStringW((BYTE *)pBlob, cbBlob, CRYPT_STRING_BASE64 | CRYPT_STRING_NOCRLF, NULL, &cchString) ? S_OK : E_FAIL;
        if (SUCCEEDED(hr))
        {
            // allocate a sufficient buffer
            PWSTR pszOut = (PWSTR)CoTaskMemAlloc(sizeof(WCHAR) * cchString);
            hr = pszOut ? S_OK : E_OUTOFMEMORY;
            if (SUCCEEDED(hr))
            {
                // convert the serialized binary blob to a string representation
                hr = CryptBinaryToStringW((BYTE *)pBlob, cbBlob, CRYPT_STRING_BASE64 | CRYPT_STRING_NOCRLF, pszOut, &cchString) ? S_OK : E_FAIL;
                if (SUCCEEDED(hr))
                {
                    *ppszOut = pszOut;
                    pszOut = NULL; // ownership transferred to *ppszOut
                }
                CoTaskMemFree(pszOut);
            }
        }
        CoTaskMemFree(pBlob);
    }
    return hr;
}

// Deserializes a string value back into PROPVARIANT form
HRESULT DeserializePropVariantFromString(PCWSTR pszIn, PROPVARIANT *ppropvar)
{
    HRESULT hr = E_FAIL;
    DWORD dwFormatUsed, dwSkip, cbBlob;

    // compute and validate the required buffer size
    if (CryptStringToBinaryW(pszIn, 0, CRYPT_STRING_BASE64, NULL, &cbBlob, &dwSkip, &dwFormatUsed) &&
        dwSkip == 0 &&
        dwFormatUsed == CRYPT_STRING_BASE64)
    {
        // allocate a buffer to hold the serialized binary blob
        BYTE *pbSerialized = (BYTE *)CoTaskMemAlloc(cbBlob);
        hr = pbSerialized ? S_OK : E_OUTOFMEMORY;
        if (SUCCEEDED(hr))
        {
            // convert the string to a serialized binary blob
            hr = CryptStringToBinaryW(pszIn, 0, CRYPT_STRING_BASE64, pbSerialized, &cbBlob, &dwSkip, &dwFormatUsed) ? S_OK : E_FAIL;
            if (SUCCEEDED(hr))
            {
                // deserialized the blob back into a PROPVARIANT value
                hr = StgDeserializePropVariant((SERIALIZEDPROPERTYVALUE *)pbSerialized, cbBlob, ppropvar);
            }
            CoTaskMemFree(pbSerialized);
        }
    }
    return hr;
}
*/