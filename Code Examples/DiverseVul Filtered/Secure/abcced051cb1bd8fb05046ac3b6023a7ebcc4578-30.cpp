Buffer<T>::~Buffer() {
  if (data()) {
    if (MemoryLoggingEnabled()) {
      RecordDeallocation();
    }
    TypedAllocator::Deallocate<T>(alloc_, static_cast<T*>(data()), elem_);
  }
}