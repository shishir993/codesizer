#pragma once

// ---------------------------------------------------
// The CodeSizer Project
// Github: https://github.com/shishir993/codesizer
// Author: Shishir Bhat [http://www.shishirbhat.com]
// The MIT License (MIT)
// Copyright (c) 2014
//
// Class CFunctionSizes
// Abstracts the task of holding function(or any symbol) name and its size,
// and provides sorting functions and write to stdout function.
//


#include "stdafx.h"

using namespace std;

class CFunctionSizes;

struct sortAsc
{
    inline bool operator () (pair<wstring,ULONGLONG> const &left, pair<wstring,ULONGLONG> const &right)
    {
        return (left.second < right.second);
    }
};

struct sortDesc
{
    inline bool operator () (pair<wstring,ULONGLONG> const &left, pair<wstring,ULONGLONG> const &right)
    {
        return (left.second > right.second);
    }
};

class CFunctionSizes
{
private:
    vector<std::pair<wstring, ULONGLONG>> _funcSizes;

public:
    CFunctionSizes()
    { }

    UINT getCount() const
    {
        return _funcSizes.size();
    }

    // Adds the specified function and its size to the list.
    // Exceptions thrown:
    //      
    HRESULT addData(PCWSTR pszFunctionName, ULONGLONG sizeInBytes);
    
    // Sort the data list in ascending or descending order in
    // preparation for writing it out.
    void sortData(bool fAscending);
    
    // Print the list to stdout.
    void dumpData();
};

