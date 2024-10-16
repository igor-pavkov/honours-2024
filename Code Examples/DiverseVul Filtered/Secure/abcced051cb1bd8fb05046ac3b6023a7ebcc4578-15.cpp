void Tensor::HostScalarTensorBufferBase::FillAllocationDescription(
    AllocationDescription* proto) const {
  proto->set_requested_bytes(size());
  proto->set_allocator_name("HostScalarTensorBuffer");
  proto->set_ptr(reinterpret_cast<uintptr_t>(data()));
}