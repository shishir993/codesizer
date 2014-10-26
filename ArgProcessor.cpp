
// ---------------------------------------------------
// The CodeSizer Project
// Github: https://github.com/shishir993/codesizer
// Author: Shishir Bhat [http://www.shishirbhat.com]
// The MIT License (MIT)
// Copyright (c) 2014
//

#include "StdAfx.h"

HRESULT CArgProcessor::ProcessArgs(_In_ int argc, _In_ WCHAR *argv[])
{
    HRESULT hr = (argc > 1) ? S_OK : E_INVALIDARG;
    if(SUCCEEDED(hr))
    {
        for(int i = 1; i < argc; ++i)
        {
            if(_wcsicmp(argv[i], L"/f") == 0)
            {
                ++i;
                if(i < argc)
                {
                    _szInputFile = argv[i];    
                }
                else
                {
                    hr = E_INVALIDARG;
                }
            }
            else if(_wcsicmp(argv[i], L"/spath") == 0)
            {
                ++i;
                if(i < argc)
                {
                    _szSearchPath = argv[i];    
                }
                else
                {
                    hr = E_INVALIDARG;
                }
            }
            else if(_wcsicmp(argv[i], L"/s") == 0)
            {
                ++i;
                if(i < argc)
                {
                    _outType |= OType_FuncSpecific;
                    hr = _ProcessSpecFunctions(argv[i]);
                }
                else
                {
                    hr = E_INVALIDARG;
                }
            }
            else if(_wcsicmp(argv[i], L"/m") == 0)
            {
                ++i;
                if(i < argc)
                {
                    _outType |= OType_FuncInModule;
                    _szModuleName = argv[i];
                }
                else
                {
                    hr = E_INVALIDARG;
                }
            }
            else if(_wcsicmp(argv[i], L"/r") == 0)
            {
                ++i;
                if(i < argc)
                {
                    _outType |= OType_FuncSubString;
                    _szSubString = argv[i];
                }
                else
                {
                    hr = E_INVALIDARG;
                }
            }
            else if(_wcsicmp(argv[i], L"/a") == 0)
            {
                _outType |= OType_FuncAll;
            }
            else
            {
                _invalidArgs.push_back(argv[i]);
            }
        }

        if(_invalidArgs.size() > 0)
        {
            hr = E_INVALIDARG;
        }
    }

    _nArgs = argc;
    _invokedProg = argv[0];

    // By default, we output all functions' size
    if(SUCCEEDED(hr) && (_outType == OType_None))
    {
        _outType = OType_FuncAll;
    }

    return hr;
}

HRESULT CArgProcessor::ValidateInputFile()
{
    HRESULT hr = S_OK;

    // Validate filepath length
    int nLen = _szInputFile.length();
    if((nLen <= 0) || (nLen >= MAX_PATH))
    {
        hr = E_INVALIDARG;
    }

    // File exists?
    if(SUCCEEDED(hr) && !PathFileExists(_szInputFile.c_str()))
    {
        hr = HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND);
    }

    if(SUCCEEDED(hr))
    {
        // PDB or EXE/DLL?
        int extIndex = _szInputFile.find_last_of(L".");
        hr = (extIndex == string::npos) ? E_INVALIDARG : S_OK;
        if(SUCCEEDED(hr))
        {
            wstring ext = _szInputFile.substr(extIndex, 4);
            if(ext == L".pdb")
            {
                _fileType = FType_PDB;
            }
            else if((ext == L".exe") || (ext == L".dll"))
            {
                _fileType = FType_DllExe;
            }
            else
            {
                _fileType = FType_None;
                hr = E_INVALIDARG;
            }
        }
    }
    return hr;
}

void CArgProcessor::PrintUsage() const
{
    if(_nArgs <= 1)
    {
        wprintf(L"Too less #arguments.\n");
    }
    else
    {
        if(_invalidArgs.size() > 0)
        {
            wprintf(L"List of unexpected args:\n");
            for each(wstring sz in _invalidArgs)
            {
                wprintf(L"%s\n", sz.c_str());
            }
        }
    }

    wprintf(
L"\nUsage:\n%s /f <path_to_PDB/EXE/DLL> [/spath <searchpath_to_PDB>] \n"
L"              [/a | /s <comma_sep_function_names> | /m <module_name> | /r <substring_to_match>]\n", _invokedProg.c_str());

    wprintf(
L"Options:\n"
L"/f        Specify the PDB/EXE/DLL path to use\n"
L"/spath    When specifying EXE/DLL, if corresponding PDB file isn't in the\n"
L"          same folder as the EXE/DLL, this is the search path for PDB.\n\n"
L"/a        Display all functions' size\n"
L"/s        Comma separated list of function names whose sizes must be displayed\n"
L"/r        Display functions that have the specified string as a sub-string. \n"
L"          This is case-sensitive.\n"
L"/m        (Not supported yet) Display functions that are in the specified module/library\n\n"

L"/a, /s, /r and /m can all be specified at the same time.\n"
L"If none of /a,/s,/r,/m is specified, it defaults to /a.\n");

}

HRESULT CArgProcessor::_ProcessSpecFunctions(_In_ PCWSTR pszFunctions)
{
    size_t nLen = wcslen(pszFunctions);
    WCHAR *pszNew = (WCHAR*)malloc((nLen + 1) * sizeof(WCHAR));
    HRESULT hr = (pszNew != nullptr) ? S_OK : E_OUTOFMEMORY;
    if(SUCCEEDED(hr))
    {
        wcscpy_s(pszNew, nLen+1, pszFunctions);

        WCHAR* pchContext = nullptr;
        WCHAR* pchStart = wcstok_s(pszNew, L",", &pchContext);
        while(pchStart != nullptr)
        {
            _specFunctions.insert(pchStart);
            pchStart = wcstok_s(NULL, L",", &pchContext);
        }

        free(pszNew);

        if(_specFunctions.size() <= 0)
        {
            return E_INVALIDARG;
        }
    }

    return hr;
}
