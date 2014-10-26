
#include "stdafx.h"

HRESULT CInputFileHandler::GetDiaSession(_In_ CComPtr<IDiaDataSource> spDiaSource, _Out_ IDiaSession **ppSession)
{
    HRESULT hr = S_OK;

    // Validate filepath length
    int nLen = _szFilepath.length();
    if((nLen <= 0) || (nLen >= MAX_PATH))
    {
        hr = E_INVALIDARG;
    }

    // File exists?
    if(SUCCEEDED(hr) && !PathFileExists(_szFilepath.c_str()))
    {
        hr = HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND);
    }

    if(SUCCEEDED(hr))
    {
        // PDB or EXE/DLL?
        int extIndex = _szFilepath.find_last_of(L".");
        hr = (extIndex == string::npos) ? E_INVALIDARG : S_OK;
        if(SUCCEEDED(hr))
        {
            wstring ext = _szFilepath.substr(extIndex, 4);
            if(ext == L".pdb")
            {
                _fileType = FType_PDB;
                hr = _OpenSessionPdb(spDiaSource, ppSession);
            }
            else if((ext == L".exe") || (ext == L".dll"))
            {
                _fileType = FType_DllExe;
                hr = _OpenSessionExeDll(spDiaSource, ppSession);
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

HRESULT CInputFileHandler::_OpenSessionPdb(_In_ CComPtr<IDiaDataSource> spDiaSource, _Out_ IDiaSession **ppSession)
{
    HRESULT hr = spDiaSource->loadDataFromPdb(_szFilepath.c_str());
    if(SUCCEEDED(hr))
    {
        hr = spDiaSource->openSession(ppSession);
    }
    return hr;
}

HRESULT CInputFileHandler::_OpenSessionExeDll(_In_ CComPtr<IDiaDataSource> spDiaSource, _Out_ IDiaSession **ppSession)
{
    return E_NOTIMPL;
}
