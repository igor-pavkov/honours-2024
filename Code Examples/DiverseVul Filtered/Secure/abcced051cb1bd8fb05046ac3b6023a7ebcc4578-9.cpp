bool Tensor::HostScalarTensorBufferBase::GetAllocatedBytes(
    size_t* out_bytes) const {
  // `this->FillAllocationDescription()` never sets allocated bytes information,
  // so we can short-circuit the construction of an `AllocationDescription`.
  return false;
}