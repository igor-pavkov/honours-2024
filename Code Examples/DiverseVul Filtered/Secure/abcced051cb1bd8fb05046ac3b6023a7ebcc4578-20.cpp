Buffer<T>::Buffer(Allocator* a, int64_t n,
                  const AllocationAttributes& allocation_attr)
    : BufferBase(a, TypedAllocator::Allocate<T>(a, n, allocation_attr)),
      elem_(n) {}