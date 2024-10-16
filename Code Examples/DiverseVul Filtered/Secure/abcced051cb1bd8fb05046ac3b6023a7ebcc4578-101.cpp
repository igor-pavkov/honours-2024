bool Tensor::FromProto(const TensorProto& proto) {
  return FromProto(get_default_cpu_allocator(), proto);
}