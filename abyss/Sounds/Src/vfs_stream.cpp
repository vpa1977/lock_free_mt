#include "../include/vfs_stream.h"


CVFSStream::CVFSStream(const std::vector<unsigned char>& data,LONG filePtr)
    : m_cRef(1)
    , m_pLoader(NULL)
	, m_Data(data)
	, m_FilePtr(filePtr)
{
	m_pFile = new VorbisFile();
	if (m_pFile->Open(m_Data) < 0)
	{
		
		delete m_pFile;
		m_pFile = NULL;
	}
}

//////////////////////////////////////////////////////////////////////////////
//
// CMyIStream::~CMyIStream
//
CVFSStream::~CVFSStream()
{
	
	delete m_pFile;
	RELEASE(m_pLoader);
}


HRESULT CVFSStream::Attach(IDirectMusicLoader *pLoader)
{

    m_pLoader = pLoader;
    m_pLoader->AddRef();

    return S_OK;
}


//////////////////////////////////////////////////////////////////////////////
//
// CMyIStream::QueryInterface
//
STDMETHODIMP CVFSStream::QueryInterface(REFIID iid, void **ppv)
{
	*ppv = NULL;

    if (iid == IID_IUnknown || iid == IID_ISequentialStream)
	{
        // ISequentialStream is the base class for IStream, which
        // provides only sequential file I/O.
        //
		*ppv = (void*)static_cast<ISequentialStream*>(this);
	}
	else if (iid == IID_IStream)
	{
        // IStream adds the concept of a file pointer to the
        // sequential stream.
        //
		*ppv = (void*)static_cast<IStream*>(this);
	}
	else if (iid == IID_IDirectMusicGetLoader) 
    {
        // This is a DirectMusic specific interface to get back
        // the loader that created this IStream.
        // 
        *ppv = (void*)static_cast<IDirectMusicGetLoader*>(this);
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
// CMyIStream::AddRef
//
STDMETHODIMP_(ULONG) CVFSStream::AddRef()
{
    return InterlockedIncrement(&m_cRef);
}

//////////////////////////////////////////////////////////////////////////////
//
// CMyIStream::Release
//
STDMETHODIMP_(ULONG) CVFSStream::Release()
{
    if (InterlockedDecrement(&m_cRef) == 0)
    {
        delete this;
        return 0;
    }

    return m_cRef;
}

//////////////////////////////////////////////////////////////////////////////
//
// CMyIStream::GetLoader 
//
// Return the loader which created this stream. Under the rules of COM, we
// have to AddRef the interface pointer first.
//
STDMETHODIMP CVFSStream::GetLoader(IDirectMusicLoader **ppLoader)
{
    m_pLoader->AddRef();
    *ppLoader = m_pLoader;
    
    return S_OK;
}

//////////////////////////////////////////////////////////////////////////////
//
// CMyIStream::Read
//
// Wraps a read call. The call is considered to have failed if not
// all the data could be read.
//
STDMETHODIMP CVFSStream::Read(void *pv, ULONG cb, ULONG *pcb)
{
    ULONG cbRead;
	

    if (pcb == NULL)
    {
        pcb = &cbRead;
    }
	
	// vorbis file is active - 
	if (m_pFile) 
	{
		const std::vector<char>& header = m_pFile->GetHeader();
		int seekPos = m_FilePtr - header.size();
		if (seekPos > 0 ) 
		{
			//m_pFile->Seek(seekPos);
			cb = m_pFile->Read(pv,cb);
			if (cb == 0 ) 
			{
				*pcb = 0;
				return -1;
			}
		}
		else
		{
			seekPos = abs(seekPos);
			int remainder = cb - seekPos;
			if (remainder <= 0 ) 
			{
				seekPos = cb;
			}
			memcpy(pv,&header[m_FilePtr], seekPos);
			if (remainder > 0 ) 
			{
				m_pFile->Seek(0);
				cb = m_pFile->Read(&((char*)pv)[seekPos],remainder) + seekPos;
			}
		}
	}
	else
	{
		if (m_FilePtr+cb >= m_Data.size() ) 
		{
			cb = m_Data.size() - m_FilePtr;
		}
		memcpy(pv,&m_Data[m_FilePtr], cb);
		/*
		for (int i = m_FilePtr ; i < m_FilePtr+cb ; i ++ ) 
		{
			((char*)pv)[i-m_FilePtr] = m_Data[i];
		}
		*/
	}
	
	m_FilePtr+= cb;

	*pcb = cb;

    return S_OK;
}


//////////////////////////////////////////////////////////////////////////////
//
// CMyIStream::Seek
//
// Move the file pointer.
//
// Be very careful here. The file pointer is 64 bits. If you are implementing
// an IStream over a medium or file system that only uses a 32-bit file pointer,
// you will have to handle negative offsets properly when casting the file 
// pointer to 32 bits. DirectMusic will make calls to move the file pointer by
// a relative offset of a negative number, and improper handling of this case
// can cause loads to fail in mysterious ways.
//
STDMETHODIMP CVFSStream::Seek(LARGE_INTEGER dlibMove, DWORD dwOrigin, ULARGE_INTEGER  *plibNewPosition)
{
    LARGE_INTEGER liNewPos;

    liNewPos.HighPart = dlibMove.HighPart;

	DWORD startPtr;
	switch (dwOrigin)
	{
		case FILE_BEGIN: startPtr = 0;
				break;
		case FILE_CURRENT: startPtr = m_FilePtr;
				break;
		case FILE_END: startPtr = m_Data.size();
				break;

	}

	
	LONG newPointer = startPtr + dlibMove.LowPart;
	liNewPos.LowPart = newPointer;
	if (newPointer < 0 || newPointer > m_Data.size() )
	{
		return E_FAIL;
	}
	m_FilePtr = newPointer;
	if (m_pFile)
	{
		long headerOff = m_pFile->GetHeader().size();
		if (headerOff <= m_FilePtr) 
		{
			m_pFile->Seek(m_FilePtr-headerOff);
		}
	}
	if (plibNewPosition)
	{
		plibNewPosition->QuadPart = liNewPos.QuadPart;
	}

    return S_OK;
}

//////////////////////////////////////////////////////////////////////////////
//
// CMyIStream::Clone
//
// Create a duplicate stream. 
//
// Two things to be careful of here are the reference counts (the new object
// must have a reference count of 1) and to properly clone the file pointer. 
//
STDMETHODIMP CVFSStream::Clone(IStream **ppstm)
{
    CVFSStream *pOther = new CVFSStream(m_Data,m_FilePtr);
	pOther->Attach(m_pLoader);
    if (pOther == NULL)
    {
        return E_OUTOFMEMORY;
    }

    *ppstm = static_cast<IStream*>(pOther);

    return S_OK;
}

//============================================================================
//
// IStream methods the DirectMusic loader doesn't use.
//
//////////////////////////////////////////////////////////////////////////////
//
// CMyIStream::Write
//
STDMETHODIMP CVFSStream::Write(const void *pv, ULONG cb, ULONG *pcb)
{
    return E_NOTIMPL;
}

//////////////////////////////////////////////////////////////////////////////
//
// CMyIStream::SetSize
//
STDMETHODIMP CVFSStream::SetSize(ULARGE_INTEGER libNewSize)
{
    return E_NOTIMPL;
}

//////////////////////////////////////////////////////////////////////////////
//
// CMyIStream::CopyTo
//
STDMETHODIMP CVFSStream::CopyTo(IStream *pstm, ULARGE_INTEGER cb, ULARGE_INTEGER  *pcbRead, ULARGE_INTEGER  *pcbWritten)
{
    return E_NOTIMPL;
}

//////////////////////////////////////////////////////////////////////////////
//
// CMyIStream::Commit
//
STDMETHODIMP CVFSStream::Commit(DWORD grfCommitFlags)
{
    return E_NOTIMPL;
}

//////////////////////////////////////////////////////////////////////////////
//
// CMyIStream::Revert
//
STDMETHODIMP CVFSStream::Revert(void)
{
    return E_NOTIMPL;
}

//////////////////////////////////////////////////////////////////////////////
//
// CMyIStream::LockRegion
//
STDMETHODIMP CVFSStream::LockRegion(ULARGE_INTEGER libOffset, ULARGE_INTEGER cb, DWORD dwLockType)
{
    return E_NOTIMPL;
}

//////////////////////////////////////////////////////////////////////////////
//
// CMyIStream::UnlockRegion
//
STDMETHODIMP CVFSStream::UnlockRegion(ULARGE_INTEGER libOffset, ULARGE_INTEGER cb, DWORD dwLockType)
{
    return E_NOTIMPL;
}

//////////////////////////////////////////////////////////////////////////////
//
// CMyIStream::Stat
//
STDMETHODIMP CVFSStream::Stat(STATSTG *pstatstg, DWORD grfStatFlag)
{
    return E_NOTIMPL;
}

