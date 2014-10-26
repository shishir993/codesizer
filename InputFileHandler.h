#pragma once

#include "stdafx.h"

using namespace std;

class CInputFileHandler
{
private:
    enum FileType
    {
        FType_None,
        FType_PDB,
        FType_DllExe
    };

    wstring _szFilepath;
    FileType _fileType;

    HRESULT _OpenSessionPdb(_In_ CComPtr<IDiaDataSource> spDiaSource, _Out_ IDiaSession **ppSession);
    HRESULT _OpenSessionExeDll(_In_ CComPtr<IDiaDataSource> spDiaSource, _Out_ IDiaSession **ppSession);

public:
    CInputFileHandler(PCWSTR pszFilepath)  :
        _szFilepath(pszFilepath)
    { }

    HRESULT CInputFileHandler::GetDiaSession(_In_ CComPtr<IDiaDataSource> spDiaSource, _Out_ IDiaSession **ppSession);
};
