
// ---------------------------------------------------
// The CodeSizer Project
// Github: https://github.com/shishir993/codesizer
// Author: Shishir Bhat [http://www.shishirbhat.com]
// The MIT License (MIT)
// Copyright (c) 2014
//
// CodeSizer.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

using namespace std;

int wmain(int argc, PWCHAR argv[])
{
    HRESULT hr = E_FAIL;

    CArgProcessor argProcessor;

    hr = argProcessor.ProcessArgs(argc, argv);
    if(SUCCEEDED(hr))
    {
        hr = argProcessor.ValidateInputFile();
        if(SUCCEEDED(hr))
        {
            hr = CoInitialize(NULL);
            if(SUCCEEDED(hr))
            {
                CComPtr<IDiaDataSource> spDiaSource;
                hr = CoCreateInstance(
                            CLSID_DiaSource, 
                            NULL, 
                            CLSCTX_INPROC_SERVER, 
                            __uuidof(IDiaDataSource), 
                            (void**)&spDiaSource);
                if(SUCCEEDED(hr))
                {
                    // The CDebugWalker object needs the IDiaSource COM object
                    // to start interacting the DIA SDK.
                    // It also needs the CArgProcessor object in order to retrieve
                    // the program arguments.
                    CDebugInfoWalker debugWalker(spDiaSource, &argProcessor);
                    hr = debugWalker.OpenSessionAndExecute();
                }

                spDiaSource.Release();
                CoUninitialize();
            }
        }
        else
        {
            logerr(L"Input file path.", hr);
        }
    }
    else
    {
        argProcessor.PrintUsage();
    }

    if(FAILED(hr) && (hr != E_INVALIDARG))
    {
        loginfo(L"Operations complete with error = 0x%08x\n", hr);
    }
	return (hr != S_OK);
}

