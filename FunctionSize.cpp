
// ---------------------------------------------------
// The CodeSizer Project
// Github: https://github.com/shishir993/codesizer
// Author: Shishir Bhat [http://www.shishirbhat.com]
// The MIT License (MIT)
// Copyright (c) 2014
//

#include "stdafx.h"

HRESULT CFunctionSizes::addData(PCWSTR pszFunctionName, ULONGLONG sizeInBytes)
{
    if((pszFunctionName == nullptr) || (sizeInBytes <= 0))
    {
        return E_INVALIDARG;
    }
    
    _funcSizes.push_back(make_pair<wstring,ULONGLONG>(wstring(pszFunctionName), sizeInBytes));
    return S_OK;
}

void CFunctionSizes::sortData(bool fAscending)
{
    if(fAscending)
    {
        sort(_funcSizes.begin(), _funcSizes.end(), sortAsc());
    }
    else
    {
        sort(_funcSizes.begin(), _funcSizes.end(), sortDesc());
    }
}

void CFunctionSizes::dumpData()
{
    wprintf(L"%-60s %s\n", L"Function Name", L"Size In Bytes");
    wprintf(L"%-60s %s\n", L"-------------", L"-------------");
    for each(auto func in _funcSizes)
    {
        wprintf(L"%-60s %llu\n", func.first.c_str(), func.second);
    }
}
