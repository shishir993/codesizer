
// ---------------------------------------------------
// The CodeSizer Project
// Github: https://github.com/shishir993/codesizer
// Author: Shishir Bhat [http://www.shishirbhat.com]
// The MIT License (MIT)
// Copyright (c) 2014
//

#include "stdafx.h"

HRESULT CDebugInfoWalker::OpenSessionAndExecute()
{
    HRESULT hr = E_UNEXPECTED;
    if(_pArgProcessor->isInputPdb())
    {
        hr = _OpenSessionPdb(_pArgProcessor->getInputFile());
    }
    else if(_pArgProcessor->isInputDllExe())
    {
        hr = _OpenSessionExeDll(_pArgProcessor->getInputFile(), _pArgProcessor->getSearchPath());
    }

    if(SUCCEEDED(hr))
    {
        // Has user asked for all functions' size?
        if(_pArgProcessor->isOutputFuncAll())
        {
            _EnumerateFunctions();
        }

        // Has user asked for sizes of specific functions?
        if(_pArgProcessor->isOutputFuncSpecific())
        {
            _EnumerateFunctions(_pArgProcessor->getSpecifiedFunctions());
        }

        // Has user asked for functions whose name have the specified sub string?
        if(_pArgProcessor->isOutputFuncSubString())
        {
            _EnumerateFunctions(_pArgProcessor->getFuncSubString());
        }

        // Has user asked for sizes of functions in specific module?
        if(_pArgProcessor->isOutputFuncInModule())
        {
            hr = E_NOTIMPL;
        }

        if(FAILED(hr))
        {
            logerr(L"Error enumerating functions", hr);
        }
    }
    else
    {
        logerr(L"Error opening session", hr);
    }

    return hr;
}

HRESULT CDebugInfoWalker::EnumerateSymbols()
{
    HRESULT hr = E_NOTIMPL;

    //CComPtr<IDiaEnumSymbols> spEnumSymbols;
    //hr = _GetTable(__uuidof(IDiaEnumSymbols), (void**)&spEnumSymbols);
    //if(SUCCEEDED(hr))
    //{
    //    wprintf(L"Symbols:: \n");
    //    ULONG celt;
    //    CComPtr<IDiaSymbol> spDiaSymbol;
    //    while(SUCCEEDED(spEnumSymbols->Next(1, &spDiaSymbol, &celt)) && (celt == 1))
    //    {
    //        BOOL isFunction;
    //        if(SUCCEEDED(spDiaSymbol->get_function(&isFunction)) && isFunction)
    //        {
    //            BSTR symName;
    //            if(SUCCEEDED(spDiaSymbol->get_undecoratedName(&symName)))
    //            {
    //                wprintf(L"%s\n", symName);
    //            }
    //        }
    //        
    //        spDiaSymbol = NULL;
    //    }
    //}
    
    return hr;
}

#pragma region PrivateFunctions

HRESULT CDebugInfoWalker::_EnumerateFunctions()
{
    CFunctionSizes funcSizes;

    CComPtr<IDiaEnumSymbols> spFuncs;
    HRESULT hr = _GetAllFunctions(&spFuncs);
    if(SUCCEEDED(hr))
    {
        ULONG celt;
        CComPtr<IDiaSymbol> spFunc;
        while(SUCCEEDED(spFuncs->Next(1, &spFunc, &celt)) && (celt == 1))
        {
            BSTR symName;
            ULONGLONG ullSize;
            if(SUCCEEDED(spFunc->get_name(&symName)) && SUCCEEDED(spFunc->get_length(&ullSize)))
            {
                funcSizes.addData(symName, ullSize);
            }
            
            spFunc = NULL;
        }
    }

    if(SUCCEEDED(hr))
    {
        funcSizes.sortData(false /* fAscending */);
        funcSizes.dumpData();
        wprintf(L"Enumerated #functions = %d\n", funcSizes.getCount());
    }

    return hr;
}

HRESULT CDebugInfoWalker::_EnumerateFunctions(_In_ wstring const & szFuncNameSubString)
{
    CFunctionSizes funcSizes;

    CComPtr<IDiaEnumSymbols> spFuncs;
    HRESULT hr = _GetAllFunctions(&spFuncs);
    if(SUCCEEDED(hr))
    {
        ULONG celt;
        CComPtr<IDiaSymbol> spFunc;
        while(SUCCEEDED(spFuncs->Next(1, &spFunc, &celt)) && (celt == 1))
        {
            BSTR symName;
            if(SUCCEEDED(spFunc->get_name(&symName)))
            {
                // Is this a function requested by the user?
                if(wcsstr(symName, szFuncNameSubString.c_str()) != nullptr)
                {
                    ULONGLONG ullSize;
                    if(SUCCEEDED(spFunc->get_length(&ullSize)))
                    {
                        funcSizes.addData(symName, ullSize);
                    }
                    else
                    {
                        logerr(L"Couldn't get size for %s", hr, symName);
                    }
                }
#ifdef _DEBUG
                else
                {
                    logdbg(L"Skipping: %s", symName);
                }
#endif
                spFunc = NULL;
            }
        }
    }

    if(SUCCEEDED(hr))
    {
        funcSizes.sortData(false /* fAscending */);
        funcSizes.dumpData();
        wprintf(L"Enumerated #functions = %d\n", funcSizes.getCount());
    }

    return hr;
}

// Display only specified functions
HRESULT CDebugInfoWalker::_EnumerateFunctions(_In_ set<wstring> const & aszFunctions)
{
    CFunctionSizes funcSizes;

    CComPtr<IDiaEnumSymbols> spFuncs;
    HRESULT hr = _GetAllFunctions(&spFuncs);
    if(SUCCEEDED(hr))
    {
        ULONG celt;
        CComPtr<IDiaSymbol> spFunc;
        while(SUCCEEDED(spFuncs->Next(1, &spFunc, &celt)) && (celt == 1))
        {
            BSTR symName;
            if(SUCCEEDED(spFunc->get_name(&symName)))
            {
                // Is this a function requested by the user?
                if(aszFunctions.find(symName) != aszFunctions.end())
                {
                    ULONGLONG ullSize;
                    hr = spFunc->get_length(&ullSize);
                    if(SUCCEEDED(hr))
                    {
                        funcSizes.addData(symName, ullSize);
                    }
                    else
                    {
                        logerr(L"Couldn't get size for %s", hr, symName);
                    }
                }
#ifdef _DEBUG
                else
                {
                    logdbg(L"Skipping: %s", symName);
                }
#endif
                spFunc = NULL;
            }
        }
    }

    if(SUCCEEDED(hr))
    {
        funcSizes.sortData(false /* fAscending */);
        funcSizes.dumpData();
        wprintf(L"Enumerated #functions = %d\n", funcSizes.getCount());
    }

    return hr;
}

HRESULT CDebugInfoWalker::_OpenSessionPdb(_In_ wstring const & szInputFile)
{
    HRESULT hr = _spDiaSource->loadDataFromPdb(szInputFile.c_str());
    if(SUCCEEDED(hr))
    {
        hr = _spDiaSource->openSession(&_spSession);
    }
    return hr;
}

HRESULT CDebugInfoWalker::_OpenSessionExeDll(_In_ wstring const & szInputFile, _In_ wstring const & szSearchPath)
{
    PCWSTR pszSearchPath = szSearchPath.empty() ? nullptr : szSearchPath.c_str();

    HRESULT hr = _spDiaSource->loadDataForExe(szInputFile.c_str(), pszSearchPath, nullptr);
    if(SUCCEEDED(hr))
    {
        hr = _spDiaSource->openSession(&_spSession);
    }

    return hr;
}

// Unused function
HRESULT CDebugInfoWalker::_GetTable(_In_ REFIID iid, _Out_ void** ppUnk)
{
    CComPtr<IDiaEnumTables> pEnumTables;
    HRESULT hr = FAILED(_spSession->getEnumTables(&pEnumTables));
    if(SUCCEEDED(hr))
    {
        CComPtr<IDiaTable> pTable;
        ULONG celt = 0;

        hr = E_NOT_SET;
        while(SUCCEEDED(pEnumTables->Next(1, &pTable, &celt)) && celt == 1)
        {
            if (pTable->QueryInterface(iid, ppUnk) == S_OK)
            {
                hr = S_OK;
                break;
            }
            pTable.Release();
        }
    }

    return hr;
}

HRESULT CDebugInfoWalker::_GetAllFunctions(_Out_ IDiaEnumSymbols **ppFunctions)
{
    *ppFunctions = nullptr;

    CComPtr<IDiaSymbol> spGlobals;
    HRESULT hr = _spSession->get_globalScope(&spGlobals);
    if(SUCCEEDED(hr))
    {
        hr = spGlobals->findChildren(SymTagFunction, NULL, nsfCaseInsensitive|nsfUndecoratedName, ppFunctions);
#ifdef _DEBUG
        if(SUCCEEDED(hr))
        {
            LONG nFuncs;
            if(SUCCEEDED((*ppFunctions)->get_Count(&nFuncs)))
            {
                logdbg(L"Total #functions = %d", nFuncs);
            }
        }
#endif
    }

    return hr;
}

#pragma endregion PrivateFunctions
