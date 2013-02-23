#ifndef VFS_STREAM
#define VFS_STREAM
// vfs based stream implemenation 
#pragma once

#include <windows.h>
#include <dmusici.h>
#include <vector>
#include "vorbis_decoder.h"

#ifndef RELEASE
#define RELEASE(x) { if(x) (x)->Release(); x = NULL; }
#endif


class CVFSStream : 
	public IStream, 
	public IDirectMusicGetLoader


{
public:
	CVFSStream( const std::vector<unsigned char>& data, LONG filePtr=0);
    ~CVFSStream();

	HRESULT Attach(IDirectMusicLoader *pLoader);

    // IUnknown methods
    //
    STDMETHODIMP QueryInterface(REFIID iid, void **ppv);
    STDMETHODIMP_(ULONG) AddRef();
    STDMETHODIMP_(ULONG) Release();

    // IDirectMusicGetLoader
    // 
    STDMETHODIMP GetLoader(IDirectMusicLoader **ppLoader);

    // IStream methods
    //
    STDMETHODIMP Read(void *pv, ULONG cb, ULONG *pcb);
    STDMETHODIMP Seek(LARGE_INTEGER dlibMove, DWORD dwOrigin, ULARGE_INTEGER  *plibNewPosition);
    STDMETHODIMP Clone(IStream **ppstm);
    STDMETHODIMP Write(const void *pv, ULONG cb, ULONG *pcb);
    STDMETHODIMP SetSize(ULARGE_INTEGER libNewSize);
    STDMETHODIMP CopyTo(IStream *pstm, ULARGE_INTEGER cb, ULARGE_INTEGER  *pcbRead, ULARGE_INTEGER  *pcbWritten);
    STDMETHODIMP Commit(DWORD grfCommitFlags);
    STDMETHODIMP Revert(void);
    STDMETHODIMP LockRegion(ULARGE_INTEGER libOffset, ULARGE_INTEGER cb, DWORD dwLockType);
    STDMETHODIMP UnlockRegion(ULARGE_INTEGER libOffset, ULARGE_INTEGER cb, DWORD dwLockType);
    STDMETHODIMP Stat(STATSTG *pstatstg, DWORD grfStatFlag);
	const std::vector<unsigned char>& GetData() { return m_Data; }
private:
	
private:
    LONG                    m_cRef;             // COM Reference count
    IDirectMusicLoader     *m_pLoader;          // Owning loader object
	std::vector<unsigned char> m_Data;
	LONG m_FilePtr;
	VorbisFile* m_pFile;

};




#endif