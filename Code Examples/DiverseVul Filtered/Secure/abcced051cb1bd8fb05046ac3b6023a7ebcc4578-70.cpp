  void RecordDeallocation() {
    LogMemory::RecordTensorDeallocation(alloc_->AllocationId(data()),
                                        alloc_->Name());
  }