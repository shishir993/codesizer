#pragma once

// ---------------------------------------------------
// The CodeSizer Project
// Github: https://github.com/shishir993/codesizer
// Author: Shishir Bhat [http://www.shishirbhat.com]
// The MIT License (MIT)
// Copyright (c) 2014
//
// Class CDebugInfoWalker
// The meat of the program's functionality is here. This class
// interacts with DIA APIs to list the functions and their sizes.
// It uses CFunctionSizes and CArgProcessor classes.
// 

#include "stdafx.h"

using namespace std;

class CDebugInfoWalker
{
private:
    CArgProcessor* _pArgProcessor;

    CComPtr<IDiaDataSource> _spDiaSource;
    CComPtr<IDiaSession> _spSession;

    HRESULT _OpenSessionPdb(_In_ wstring const & szInputFile);
    HRESULT _OpenSessionExeDll(_In_ wstring const & szInputFile, _In_ wstring const & szSearchPath);

    HRESULT _GetTable(_In_ REFIID iid, _Out_ void** ppUnk);
    HRESULT _GetAllFunctions(_Out_ IDiaEnumSymbols **ppFunctions);

public:
    CDebugInfoWalker(_In_ IDiaDataSource* pDiaSource, _In_ CArgProcessor* pArgProcessor) :
        _spDiaSource(pDiaSource),
        _pArgProcessor(pArgProcessor)
    { }

    HRESULT OpenSessionAndExecute();

    // Display all symbols within the global scope
    HRESULT EnumerateSymbols();
        
    // Display only functions within the global scope
    HRESULT _EnumerateFunctions();

    // Display all functions in global scope whose name have the specified sub string
    HRESULT _EnumerateFunctions(_In_ wstring const & szFuncNameSubString);

    // Display only functions within the specified library/object
    //HRESULT _EnumerateFunctions(_In_ wstring szLibObjName);

    // Display only specified functions
    HRESULT _EnumerateFunctions(_In_ set<wstring> const & aszFunctions);
    
};
