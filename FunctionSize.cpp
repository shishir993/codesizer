
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
    
    _funcSizes.emplace_back(pszFunctionName, sizeInBytes);
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
    wprintf(L"Function Name,Size In Bytes\n");
    for (auto& func : _funcSizes)
    {
        auto& fname = func.first;
        for (auto& c : fname)
        {
            if (c == L',')
            {
                c = L';';
            }
        }
        wprintf(L"%s,%llu\n", func.first.c_str(), func.second);
    }
}
