#ifndef DM_LOADER_HPP

#define DM_LOADER_HPP

#pragma once

#include <windows.h>
#include <dmusici.h>
#ifndef RELEASE
#define RELEASE(x) { if(x) (x)->Release(); x = NULL; }
#endif



class CObjectRef
{
public:
    CObjectRef() { m_pNext = NULL; m_pObject = NULL; m_pwsFileName = NULL;};
    ~CObjectRef() { delete [] m_pwsFileName; };
    CObjectRef *    m_pNext;
    WCHAR *         m_pwsFileName;
    GUID            m_guidObject;
    IDirectMusicObject *    m_pObject;
};

class DMLoader : public IDirectMusicLoader
{
public:
    DMLoader();
    ~DMLoader();
    HRESULT Init();

    // IUnknown methods
    //
    STDMETHODIMP QueryInterface(REFIID iid, void **ppv);
    STDMETHODIMP_(ULONG) AddRef();
    STDMETHODIMP_(ULONG) Release();

    // IDirectMusicLoader methods
    //
    STDMETHODIMP GetObject(LPDMUS_OBJECTDESC pDesc, REFIID riid, LPVOID FAR *ppv);
    STDMETHODIMP SetObject(LPDMUS_OBJECTDESC pDesc);
    STDMETHODIMP SetSearchDirectory(REFGUID rguidClass, WCHAR *pwzPath, BOOL fClear);
    STDMETHODIMP ScanDirectory(REFGUID rguidClass, WCHAR *pwzFileExtension, WCHAR *pwzScanFileName);
    STDMETHODIMP CacheObject(IDirectMusicObject *pObject);
    STDMETHODIMP ReleaseObject(IDirectMusicObject *pObject);
    STDMETHODIMP ClearCache(REFGUID rguidClass);
    STDMETHODIMP EnableCache(REFGUID rguidClass, BOOL fEnable);
    STDMETHODIMP EnumObject(REFGUID rguidClass, DWORD dwIndex, LPDMUS_OBJECTDESC pDesc);

private:
    LONG                    m_cRef;             // COM Reference count
    CObjectRef *            m_pObjectList;      // List of already loaded objects (the cache.)
};



#endif