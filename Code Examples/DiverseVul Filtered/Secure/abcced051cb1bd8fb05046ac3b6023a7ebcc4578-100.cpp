Tensor::Tensor(DataType type, const TensorShape& shape)
    : Tensor(get_default_cpu_allocator(), type, shape) {}