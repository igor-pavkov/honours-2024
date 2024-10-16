bool TensorBuffer::GetAllocatedBytes(size_t* out_bytes) const {
  AllocationDescription allocation_description;
  FillAllocationDescription(&allocation_description);
  if (allocation_description.allocated_bytes() > 0) {
    *out_bytes = allocation_description.allocated_bytes();
    return true;
  } else {
    return false;
  }
}