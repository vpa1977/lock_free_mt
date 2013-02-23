#ifndef ALLOCATOR_HPP

#define ALLOCATOR_HPP
#include "stdafx.h"
#include <assert.h>
#include <hash_map>
#include "alloc_queue.h"
#include <vector>

#define MALLOC_ALIGNMENT 8
/* The byte and bit size of a size_t */
#define SIZE_T_SIZE         (sizeof(size_t))
#define SIZE_T_BITSIZE      (sizeof(size_t) << 3)

/* Some constants coerced to size_t */
/* Annoying but necessary to avoid errors on some plaftorms */
#define SIZE_T_ZERO         ((size_t)0)
#define SIZE_T_ONE          ((size_t)1)
#define SIZE_T_TWO          ((size_t)2)
#define TWO_SIZE_T_SIZES    (SIZE_T_SIZE<<1)
#define FOUR_SIZE_T_SIZES   (SIZE_T_SIZE<<2)
#define SIX_SIZE_T_SIZES    (FOUR_SIZE_T_SIZES+TWO_SIZE_T_SIZES)
#define HALF_MAX_SIZE_T     (MAX_SIZE_T / 2U)

/* The bit mask value corresponding to MALLOC_ALIGNMENT */
#define CHUNK_ALIGN_MASK    (MALLOC_ALIGNMENT - SIZE_T_ONE)

/* True if address a has acceptable alignment */
#define is_aligned(A)       (((size_t)((A)) & (CHUNK_ALIGN_MASK)) == 0)

/* the number of bytes to offset an address to align it */
#define align_offset(A)\
 ((((size_t)(A) & CHUNK_ALIGN_MASK) == 0)? 0 :\
  ((MALLOC_ALIGNMENT - ((size_t)(A) & CHUNK_ALIGN_MASK)) & CHUNK_ALIGN_MASK))



struct chunk
{
	chunk() : m_pblock(NULL), m_counter(0), m_block_pointer(0)
	{
	};
	char * m_pblock;
	volatile long m_block_pointer;
	volatile long m_counter;
	void Increase()
	{
		InterlockedIncrement(&m_counter);
	}
	void Decrease()
	{
		InterlockedDecrement(&m_counter);
	}
};
class chain_allocator
{
public:
	chain_allocator(long bs)
	{
		m_block_size = bs;
		m_max_block = 0;
		m_pcurrent_chunk = NULL;
		claim_block();
	}

	chunk * get_chunk()
	{
		return m_pcurrent_chunk;
	}
	// called from thread 1
	void *allocate(size_t n);
	
	// called from thread 2
	void deallocate(chunk *p )
	{
		p->Decrease();
	}
private:
	void claim_block();

//	AllocQueue<chunk*> m_free_chunks;
	std::vector<chunk*> m_free_chunks;
	chunk* m_pcurrent_chunk;
	long m_block_size;
	int m_max_block;
};

#endif