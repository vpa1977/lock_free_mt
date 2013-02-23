#include "abstract_feature.h"

void
CAbstractFeature::AddFragmentsInternal( INVLink* in_pLinker, const char* in_pFileName ) {
	assert(in_pFileName);
	HANDLE hFile = CreateFile(in_pFileName, GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL);
	assert( INVALID_HANDLE_VALUE != hFile );
	DWORD dwFileSize = GetFileSize(hFile, NULL);
	void* pFragments = (void*)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, dwFileSize);
	ReadFile(hFile, pFragments, dwFileSize, &dwFileSize, NULL);
	CloseHandle(hFile);
	HRESULT Res = in_pLinker->AddFragments(pFragments, dwFileSize);
	assert( Res == S_OK );
	HeapFree(GetProcessHeap(), 0, (void*)pFragments);
}




