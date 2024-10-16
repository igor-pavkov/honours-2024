  explicit BufferBase(Allocator* alloc, void* data_ptr)
      : TensorBuffer(data_ptr), alloc_(alloc) {}