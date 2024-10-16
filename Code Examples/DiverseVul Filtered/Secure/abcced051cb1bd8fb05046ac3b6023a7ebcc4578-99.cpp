  bool GetAllocatedBytes(size_t* out_bytes) const override {
    if (alloc_->TracksAllocationSizes()) {
      *out_bytes = alloc_->AllocatedSize(data());
      return *out_bytes > 0;
    } else {
      return false;
    }
  }