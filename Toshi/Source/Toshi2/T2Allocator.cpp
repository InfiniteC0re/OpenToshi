#include "ToshiPCH.h"
#include "T2Allocator.h"

TOSHI_NAMESPACE_START

void* T2GlobalAllocator::Malloc(size_t size)
{
	return TMalloc(size);
}

void* T2GlobalAllocator::Malloc(size_t size, size_t alignment)
{
	return TMemalign(alignment, size);
}

void T2GlobalAllocator::Free(void* ptr)
{
	TFree(ptr);
}

TBOOL T2GlobalAllocator::CanAllocate(size_t size)
{
	return TTRUE;
}

void* T2GlobalAllocator::TryMalloc(size_t size)
{
	return TMalloc(size);
}

void* T2GlobalAllocator::TryMalloc(size_t size, size_t alignment)
{
	return TMemalign(alignment, size);
}

size_t T2GlobalAllocator::GetUsedSize()
{
	return 0;
}

size_t T2GlobalAllocator::GetCapacity()
{
	return 0;
}

// Global allocator
T2GlobalAllocator  g_oGlobalAllocator;
T2GlobalAllocator* GetGlobalAllocator()
{
	return &g_oGlobalAllocator;
}

TOSHI_NAMESPACE_END
