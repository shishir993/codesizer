#pragma once

// ---------------------------------------------------
// The CodeSizer Project
// Github: https://github.com/shishir993/codesizer
// Author: Shishir Bhat [http://www.shishirbhat.com]
// The MIT License (MIT)
// Copyright (c) 2014
//
// Class CArgProcessor
// Processes(and validates) the command-line arguments to the program.
// Provides a way for users of this class to retrieve the 
// programs arguments in order to decide what to execute.
//

#include "stdafx.h"

#define OType_None              0x0000
#define OType_FuncAll           0x0001
#define OType_FuncSpecific      0x0002
#define OType_FuncInModule      0x0040
#define OType_FuncSubString     0x0080
#define OType_FuncPublic        0x0100

using namespace std;

class CArgProcessor
{
private:
    enum FileType
    {
        FType_None,
        FType_PDB,
        FType_DllExe
    };

    FileType _fileType;
    wstring _szInputFile;
    wstring _szSearchPath;

    int _nArgs;
    wstring _invokedProg;
    vector<wstring> _invalidArgs;

    UINT _outType;
    wstring _szModuleName;
    wstring _szSubString;
    set<wstring> _specFunctions;

    // Private functions
    HRESULT _ProcessSpecFunctions(_In_ PCWSTR pszFunctions);

public:
    CArgProcessor() :
        _outType(OType_None),
        _fileType(FType_None),
        _nArgs(0)
    { }

    HRESULT ProcessArgs(_In_ int argc, _In_ WCHAR *argv[]);
    HRESULT ValidateInputFile();

    bool isInputPdb() const
    {
        return (_fileType == FType_PDB);
    }

    bool isInputDllExe() const
    {
        return (_fileType == FType_DllExe);
    }

    wstring const & getInputFile() const
    {
        return _szInputFile;
    }

    wstring const & getSearchPath() const
    {
        return _szSearchPath;
    }

    bool isOutputFuncInModule() const
    {
        return (_outType & OType_FuncInModule) ? true : false;
    }

    bool isOutputFuncSpecific() const
    {
        return (_outType & OType_FuncSpecific) ? true : false;
    }

    bool isOutputFuncAll() const
    {
        return (_outType & OType_FuncAll) ? true : false;
    }

    bool isOutputFuncPublic() const
    {
        return (_outType & OType_FuncPublic) ? true : false;
    }

    bool isOutputFuncSubString() const
    {
        return (_outType & OType_FuncSubString) ? true : false;
    }

    wstring const & getModuleName() const
    {
        return _szModuleName;
    }

    wstring const & getFuncSubString() const
    {
        return _szSubString;
    }

    set<wstring> const & getSpecifiedFunctions() const
    {
        return _specFunctions;
    }

    void PrintUsage() const;
};

