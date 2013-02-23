#include "../include/dm_loader.h"
#include "../include/vfs_stream.h"

//////////////////////////////////////////////////////////////////////////////
//
// CMyLoader::CMyLoader
//
DMLoader::DMLoader()
    : m_cRef(1)
    , m_pObjectList(NULL)
{
}

//////////////////////////////////////////////////////////////////////////////
//
// CMyLoader::~CMyLoader
//
DMLoader::~DMLoader()
{
    while (m_pObjectList)
    {
        CObjectRef * pObject = m_pObjectList;
        m_pObjectList = pObject->m_pNext;
        if (pObject->m_pObject)
        {
            pObject->m_pObject->Release();
        }
        delete pObject;
    }
}


//////////////////////////////////////////////////////////////////////////////
//
// CMyLoader::Init
//
// Find and cache an object pointer for GM.DLS. If your title does not use 
// any content that references the default DLS collection, you do not need
// this code.
//

//#define NEED_GM_SET

HRESULT DMLoader::Init()
{
//    USES_CONVERSION;

    HRESULT hr = S_OK;
/*
    // If support for the GM set is desired, create a loader,
    // get the GM.dls collection from it, and then release that loader.
    // We'll keep a reference to the GM set by placing it in the cache list.

#ifdef NEED_GM_SET
    IDirectMusicLoader *pLoader;
    hr = CoCreateInstance(            
        CLSID_DirectMusicLoader,
        NULL,            
        CLSCTX_INPROC,             
        IID_IDirectMusicLoader,
        (void**)&pLoader); 
    if (SUCCEEDED(hr))
    {
        DMUS_OBJECTDESC ObjDesc;     
        IDirectMusicObject* pGMSet = NULL; 
        ObjDesc.guidClass = CLSID_DirectMusicCollection;
        ObjDesc.guidObject = GUID_DefaultGMCollection;
        ObjDesc.dwSize = sizeof(DMUS_OBJECTDESC);
        ObjDesc.dwValidData = DMUS_OBJ_CLASS | DMUS_OBJ_OBJECT;
        hr = pLoader->GetObject( &ObjDesc,
                IID_IDirectMusicObject, (void**) &pGMSet );
        if (SUCCEEDED(hr))
        {
            CObjectRef *pRef = new CObjectRef();
            if (pRef)
            {
                pRef->m_guidObject = GUID_DefaultGMCollection;
                pRef->m_pNext = m_pObjectList;
                m_pObjectList = pRef;
                pRef->m_pObject = pGMSet;
                pGMSet->AddRef();
            }
            pGMSet->Release();
        }
        pLoader->Release();
    }
#endif
	*/
    return hr;    
}

//////////////////////////////////////////////////////////////////////////////
//
// CMyLoader::QueryInterface
//
STDMETHODIMP DMLoader::QueryInterface(REFIID iid, void **ppv)
{
    *ppv = NULL;

    if (iid == IID_IUnknown || iid == IID_IDirectMusicLoader)
    {
        *ppv = (void*)static_cast<IDirectMusicLoader*>(this);
    }
    else
    {
        return E_NOINTERFACE;
    }

    AddRef();
    return S_OK;
}

//////////////////////////////////////////////////////////////////////////////
//
// CMyLoader::AddRef
//
STDMETHODIMP_(ULONG) DMLoader::AddRef()
{
    return InterlockedIncrement(&m_cRef);
}

//////////////////////////////////////////////////////////////////////////////
//
// CMyLoader::Release
//
STDMETHODIMP_(ULONG) DMLoader::Release()
{
    LONG ulCount = InterlockedDecrement(&m_cRef);
    if (ulCount <= 0)
    {
        delete this;
    }
    return (ULONG) ulCount;
}

//////////////////////////////////////////////////////////////////////////////
//
// CMyLoader::GetObject
//
// This is the workhorse method of the loader. The DMUS_OBJECTDESC structure
// describes the object to be loaded. There are basically three cases that
// must be handled, based on the following three flags in dwValidData:
//
// 1. DMUS_OBJ_OBJECT. This indicates that the content 
//    is being referenced by GUID. The only time you *must* pay attention 
//    to this type of load is GUID_DefaultGMCollection. Here we satisfy
//    this request with a cached pointer to GM.DLS.
//
// 2. DMUS_OBJ_FILENAME. This is a request to load by filename. Note that
//    referenced content is loaded by just the filename, so you must have
//    a scheme for locating the directory the content is in. The default 
//    DirectMusic loader uses SetSearchDirectory to accomplish this; however,
//    you can find the content in any way you like.
//
// 3. DMUS_OBJ_STREAM. You will receive this load request when content 
//    is embedded in a container object within another file. The key
//    thing to note here is that embedded content is loaded recursively, 
//    and the caller expects the stream pointer to be unchanged on return
//    from the nested load. Therefore you must clone the stream and load on
//    the cloned stream. You cannot be guaranteed that Load will restore the
//    stream pointer on all media types.
// 
//
STDMETHODIMP DMLoader::GetObject(LPDMUS_OBJECTDESC pDesc, REFIID riid, LPVOID FAR *ppv)
{
//    USES_CONVERSION;

    HRESULT hr;
    IDirectMusicObject *pObject = NULL;
    MUSIC_TIME mt = 0;

    // First check whether the object has already been loaded. 
    // In this case, we are looking for only the object GUID or file name.
    // If the loader sees that the object is already loaded, it 
    // returns a pointer to it and increments the reference. 
    // It is very important to keep the previously loaded objects
    // cached in this way. Otherwise, objects like DLS collections will get loaded
    // multiple times with a very great expense in memory and efficiency.
    // This is primarily an issue when objects reference each other: for example,
    // when segments reference style and collection objects. 

    CObjectRef * pObjectRef = NULL;

    // Scan by GUID.

    if (pDesc->dwValidData & DMUS_OBJ_OBJECT)
    {
        for (pObjectRef = m_pObjectList; pObjectRef; pObjectRef = pObjectRef->m_pNext)
        {
            if (pDesc->guidObject == pObjectRef->m_guidObject)
            {
                break;
            }
        }
    }

    // Scan by file name.

    else if (pDesc->dwValidData & DMUS_OBJ_FILENAME)
    {
		return E_FAIL;
		/*
        for (pObjectRef = m_pObjectList; pObjectRef; pObjectRef = pObjectRef->m_pNext)
        {
            if (pObjectRef->m_pwsFileName && 
                !wcscmp(pDesc->wszFileName,pObjectRef->m_pwsFileName))
            {
                break;
            }
        }*/
    }

    // If the object was found, make sure it supports the requested 
    // interface, and add a reference, but don't reload it.


    if (pObjectRef)
    {
        hr = E_FAIL;
        if (pObjectRef->m_pObject)
        {
            hr = pObjectRef->m_pObject->QueryInterface( riid, ppv );
        }
        return hr;
    }

    // We didn't find it in the cache, so now we must create it.
    // First the given class must be creatable and must support IDirectMusicObject.
    
    hr = CoCreateInstance(pDesc->guidClass, NULL,CLSCTX_INPROC_SERVER, IID_IDirectMusicObject, (void **)&pObject);
    if (FAILED(hr))
    {
        return hr;
    }

    if (pDesc->dwValidData & DMUS_OBJ_FILENAME)
    {
		return E_FAIL;
		/*
        // Load from a file. Make a stream based on the file and load from
        // it. 
        //
        // Though we RELEASE the stream pointer at the end, DirectMusic may
        // hold on to the stream if needed (such as for DLS collections or
        // streamed waves).
        //
        WCHAR wzFileName[MAX_PATH];
        WCHAR wzExt[_MAX_EXT];

        _wmakepath(wzFileName, NULL, m_wzSearchPath, pDesc->wszFileName, NULL);
        _wsplitpath(wzFileName, NULL, NULL, NULL, wzExt);

        CMyIStream *pStream = new CMyIStream;
        if (pStream == NULL)
        {
            hr = E_OUTOFMEMORY;
        }

        if (SUCCEEDED(hr))
        {
            hr = pStream->Attach(W2CT(wzFileName), this);
        }

        if (SUCCEEDED(hr) && _wcsicmp(wzExt, L".wav") == 0 && riid == IID_IDirectMusicSegment8)
        {
            // This is a workaround for a missing feature in DirectX 8.0
            // See the comments for MusicTimeFromWav.
            //
            mt = MusicTimeFromWav(pStream);
        }

        IPersistStream *pPersistStream = NULL;            
        if (SUCCEEDED(hr))
        {
            hr = pObject->QueryInterface(IID_IPersistStream, (void**)&pPersistStream);
        }

        if (SUCCEEDED(hr))
        {
            hr = pPersistStream->Load(pStream);
        }

        RELEASE(pStream);
        RELEASE(pPersistStream);
		*/
    }
    else if (pDesc->dwValidData & DMUS_OBJ_STREAM)
    {
        // Loading by stream. 
        //
		
        IStream *pClonedStream = NULL;
        IPersistStream *pPersistStream = NULL;
		((CVFSStream*) pDesc->pStream)->Attach(this);
        hr = pObject->QueryInterface(IID_IPersistStream, (void**)&pPersistStream);
        if (SUCCEEDED(hr))
        {
            hr = pDesc->pStream->Clone(&pClonedStream);
        }
		
        if (SUCCEEDED(hr))
        {
            hr = pPersistStream->Load(pClonedStream);
        }

        RELEASE(pPersistStream);
        RELEASE(pClonedStream);
    }
    else
    {
        // No way we understand to reference the object.
        //
        hr = E_FAIL;
    }

    if (SUCCEEDED(hr))
    {
        // If we succeeded in loading it, keep a pointer to it, 
        // AddRef it, and keep the GUID for finding it next time.
        // To get the GUID, call ParseDescriptor on the object and
        // it will fill in the fields it knows about, including the
        // GUID.
        // Note that this only applies to wave, DLS, and style objects.
        // You may wish to add or remove other object types.

        if ((pDesc->guidClass == CLSID_DirectMusicStyle) ||
            (pDesc->guidClass == CLSID_DirectSoundWave) ||
            (pDesc->guidClass == CLSID_DirectMusicCollection))
        {
            DMUS_OBJECTDESC DESC;
            memset((void *)&DESC,0,sizeof(DESC));
            DESC.dwSize = sizeof (DMUS_OBJECTDESC); 
			
            pObject->GetDescriptor(&DESC);
            if ((DESC.dwValidData & DMUS_OBJ_OBJECT) || (pDesc->dwValidData & DMUS_OBJ_FILENAME))
            {
                CObjectRef * pObjectRef = new CObjectRef;
                if (pObjectRef)
                {
                    pObjectRef->m_guidObject = DESC.guidObject;
                    if (pDesc->dwValidData & DMUS_OBJ_FILENAME)
                    {
                        pObjectRef->m_pwsFileName = new WCHAR[wcslen(pDesc->wszFileName)+1];
                        wcscpy(pObjectRef->m_pwsFileName,pDesc->wszFileName);
                    }
                    pObjectRef->m_pNext = m_pObjectList;
                    m_pObjectList = pObjectRef;
                    pObjectRef->m_pObject = pObject;
                    pObject->AddRef();
                }
            }
        }
        hr = pObject->QueryInterface( riid, ppv );
    }

    if (SUCCEEDED(hr) && riid == IID_IDirectMusicSegment8 && mt)
    {
        IDirectMusicSegment *pSegment = (IDirectMusicSegment8*)(*ppv);

        hr = pSegment->SetLength(mt);
    }

    RELEASE(pObject);

    return hr;
}

//////////////////////////////////////////////////////////////////////////////
//
// CMyLoader::SetSearchDirectory
//
// We only implement this by convention for the example. If you have another 
// method of locating your content (for example, the filename indicates which
// file system the content is embedded in) you can use that instead.

STDMETHODIMP DMLoader::SetSearchDirectory(REFCLSID rguidClass, WCHAR *pwzPath, BOOL fClear)                        
{
    //wcscpy(m_wzSearchPath, pwzPath);

    return S_OK;
}

//----------------------------------------------------------------------------
//
// Methods you don't need to implement.

STDMETHODIMP DMLoader::SetObject(LPDMUS_OBJECTDESC pDESC)
{
    return E_NOTIMPL;
}

STDMETHODIMP DMLoader::ScanDirectory(REFCLSID rguidClass, WCHAR *pszFileExtension, WCHAR *pszCacheFileName)
{
    return E_NOTIMPL;
}

STDMETHODIMP DMLoader::CacheObject(IDirectMusicObject * pObject)
{
    return E_NOTIMPL;
}

STDMETHODIMP DMLoader::ReleaseObject(IDirectMusicObject * pObject)
{
    return E_NOTIMPL;
}

STDMETHODIMP DMLoader::ClearCache(REFCLSID rguidClass)
{
    return E_NOTIMPL;
}

STDMETHODIMP DMLoader::EnableCache(REFCLSID rguidClass, BOOL fEnable)          
{
    return E_NOTIMPL;
}

STDMETHODIMP DMLoader::EnumObject(REFCLSID rguidClass, DWORD dwIndex, LPDMUS_OBJECTDESC pDESC)
{
    return E_NOTIMPL;
}

