Buffer<T>::Buffer(Allocator* a, int64_t n)
    : BufferBase(a, TypedAllocator::Allocate<T>(a, n, AllocationAttributes())),
      elem_(n) {}