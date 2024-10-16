  void FillAllocationDescription(AllocationDescription* proto) const override {
    void* data_ptr = data();
    int64_t rb = size();
    proto->set_requested_bytes(rb);
    proto->set_allocator_name(alloc_->Name());
    proto->set_ptr(reinterpret_cast<uintptr_t>(data_ptr));
    if (alloc_->TracksAllocationSizes()) {
      int64_t ab = alloc_->AllocatedSize(data_ptr);
      proto->set_allocated_bytes(ab);
      int64_t id = alloc_->AllocationId(data_ptr);
      if (id > 0) {
        proto->set_allocation_id(id);
      }
      if (RefCountIsOne()) {
        proto->set_has_single_reference(true);
      }
    }
  }