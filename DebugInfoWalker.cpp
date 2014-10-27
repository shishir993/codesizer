
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
        bool isPublicOnly = _pArgProcessor->isOutputFuncPublic();

        // Has user asked for all functions' size?
        if(_pArgProcessor->isOutputFuncAll())
        {
            _EnumerateFunctions(isPublicOnly);
        }

        // Has user asked for sizes of specific functions?
        if(_pArgProcessor->isOutputFuncSpecific())
        {
            _EnumerateFunctions(_pArgProcessor->getSpecifiedFunctions(), isPublicOnly);
        }

        // Has user asked for functions whose name have the specified sub string?
        if(_pArgProcessor->isOutputFuncSubString())
        {
            _EnumerateFunctions(_pArgProcessor->getFuncSubString(), isPublicOnly);
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
    //CComPtr<IDiaEnumSymbols> spEnumSymbols;
    //HRESULT hr = _GetAllSymbols(&spEnumSymbols);
    //if(SUCCEEDED(hr))
    //{
    //    wprintf(L"Symbols:: \n");
    //    ULONG celt;
    //    CComPtr<IDiaSymbol> spDiaSymbol;
    //    while(SUCCEEDED(spEnumSymbols->Next(1, &spDiaSymbol, &celt)) && (celt == 1))
    //    {
    //        BSTR symName;
    //        if(spDiaSymbol->get_name(&symName) == S_OK)
    //        {
    //            wprintf(L"%s\n", symName);
    //            SysFreeString(symName);
    //        }
    //        spDiaSymbol.Release();
    //    }
    //}
    
    return E_NOTIMPL;
}

#pragma region PrivateFunctions

HRESULT CDebugInfoWalker::_EnumerateFunctions(bool fPublicOnly)
{
    CFunctionSizes funcSizes;

    wprintf(L"%s ::\n", fPublicOnly ? L"Public Functions" : L"Functions");

    CComPtr<IDiaEnumSymbols> spFuncs;

    HRESULT hr = fPublicOnly ? _GetAllPublicSymbols(&spFuncs) : _GetAllFunctions(&spFuncs);
    if(SUCCEEDED(hr))
    {
        ULONG celt;
        CComPtr<IDiaSymbol> spFunc;
        while(SUCCEEDED(spFuncs->Next(1, &spFunc, &celt)) && (celt == 1))
        {
            // If we are to display only public functions, then check if this
            // is a function or not.
            if(fPublicOnly)
            {
                BOOL isFunc;
                if((spFunc->get_function(&isFunc) == S_OK) && (isFunc == FALSE))
                {
                    // Not a function, continue
                    spFunc.Release();
                    continue;
                }
            }

            _AddFuncToList(funcSizes, spFunc, L"");
            spFunc.Release();
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

HRESULT CDebugInfoWalker::_EnumerateFunctions(_In_ wstring const & szFuncNameSubString, bool fPublicOnly)
{
    CFunctionSizes funcSizes;

    CComPtr<IDiaEnumSymbols> spFuncs;

    HRESULT hr = fPublicOnly ? _GetAllPublicSymbols(&spFuncs) : _GetAllFunctions(&spFuncs);
    if(SUCCEEDED(hr))
    {
        ULONG celt;
        CComPtr<IDiaSymbol> spFunc;
        while(SUCCEEDED(spFuncs->Next(1, &spFunc, &celt)) && (celt == 1))
        {
            // If we are to display only public functions, then check if this
            // is a function or not.
            if(fPublicOnly)
            {
                BOOL isFunc;
                if((spFunc->get_function(&isFunc) == S_OK) && (isFunc == FALSE))
                {
                    // Not a function, continue
                    spFunc.Release();
                    continue;
                }
            }

            _AddFuncToList(funcSizes, spFunc, szFuncNameSubString);
            spFunc.Release();
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
HRESULT CDebugInfoWalker::_EnumerateFunctions(_In_ set<wstring> const & aszFunctions, bool fPublicOnly)
{
    CFunctionSizes funcSizes;

    CComPtr<IDiaEnumSymbols> spFuncs;
    
    HRESULT hr = fPublicOnly ? _GetAllPublicSymbols(&spFuncs) : _GetAllFunctions(&spFuncs);
    if(SUCCEEDED(hr))
    {
        ULONG celt;
        CComPtr<IDiaSymbol> spFunc;
        while(SUCCEEDED(spFuncs->Next(1, &spFunc, &celt)) && (celt == 1))
        {
            // If we are to display only public functions, then check if this
            // is a function or not.
            if(fPublicOnly)
            {
                BOOL isFunc;
                if((spFunc->get_function(&isFunc) == S_OK) && (isFunc == FALSE))
                {
                    // Not a function, continue
                    spFunc.Release();
                    continue;
                }
            }

            _AddFuncToList(funcSizes, spFunc, aszFunctions);
            spFunc.Release();
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
            if((*ppFunctions)->get_Count(&nFuncs) == S_OK)
            {
                logdbg(L"Total #functions = %d", nFuncs);
            }
        }
#endif
    }

    return hr;
}

HRESULT CDebugInfoWalker::_GetAllSymbols(_Out_ IDiaEnumSymbols **ppAllSymbols)
{
    *ppAllSymbols = nullptr;

    CComPtr<IDiaSymbol> spGlobals;
    HRESULT hr = _spSession->get_globalScope(&spGlobals);
    if(SUCCEEDED(hr))
    {
        hr = spGlobals->findChildren(SymTagNull, NULL, nsfCaseInsensitive|nsfUndecoratedName, ppAllSymbols);
#ifdef _DEBUG
        if(SUCCEEDED(hr))
        {
            LONG nSyms;
            if((*ppAllSymbols)->get_Count(&nSyms) == S_OK)
            {
                logdbg(L"Total #symbols = %d", nSyms);
            }
        }
#endif
    }

    return hr;
}

HRESULT CDebugInfoWalker::_GetAllPublicSymbols(_Out_ IDiaEnumSymbols **ppAllSymbols)
{
    *ppAllSymbols = nullptr;

    CComPtr<IDiaSymbol> spGlobals;
    HRESULT hr = _spSession->get_globalScope(&spGlobals);
    if(SUCCEEDED(hr))
    {
        hr = spGlobals->findChildren(SymTagPublicSymbol, NULL, nsfCaseInsensitive|nsfUndecoratedName, ppAllSymbols);
#ifdef _DEBUG
        if(SUCCEEDED(hr))
        {
            LONG nSyms;
            if((*ppAllSymbols)->get_Count(&nSyms) == S_OK)
            {
                logdbg(L"Total #symbols = %d", nSyms);
            }
        }
#endif
    }

    return hr;
}

HRESULT CDebugInfoWalker::_AddFuncToList(
    _In_ CFunctionSizes & funcList, 
    _In_ IDiaSymbol *pSymbol, 
    _In_ wstring const & funcNameToMatch)
{
    BSTR symName;

    HRESULT hr = pSymbol->get_name(&symName);
    if(hr == S_OK)
    {
        if(!funcNameToMatch.empty() && (wcsstr(symName, funcNameToMatch.c_str()) == nullptr))
        {
            // Requested for substring match but no match found
            logdbg(L"Skipping: %s", symName);

            SysFreeString(symName);
            return S_FALSE;
        }

        ULONGLONG ullSize;
        hr = pSymbol->get_length(&ullSize);
        if(hr == S_OK)
        {
            funcList.addData(symName, ullSize);
        }
        SysFreeString(symName);
    }
    return hr;
}

HRESULT CDebugInfoWalker::_AddFuncToList(
    _In_ CFunctionSizes & funcList, 
    _In_ IDiaSymbol *pSymbol, 
    _In_ set<wstring> const & aszFuncNamesToMatch)
{
    BSTR symName;

    HRESULT hr = pSymbol->get_name(&symName);
    if(hr == S_OK)
    {
        // Is this a function requested by the user?
        if(aszFuncNamesToMatch.find(symName) != aszFuncNamesToMatch.end())
        {
            ULONGLONG ullSize;
            if(pSymbol->get_length(&ullSize) == S_OK)
            {
                funcList.addData(symName, ullSize);
            }
        }
#ifdef _DEBUG
        else
        {
            logdbg(L"Skipping: %s", symName);
        }
#endif
        SysFreeString(symName);
    }
    return hr;
}

#pragma endregion PrivateFunctions
