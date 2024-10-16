  SubBuffer(TensorBuffer* buf, int64_t delta, int64_t n)
      : TensorBuffer(buf->base<T>() + delta),
        root_(buf->root_buffer()),
        elem_(n) {
    // Sanity check. The caller should ensure the sub buffer is valid.
    CHECK_LE(root_->base<T>(), this->base<T>());
    T* root_limit = root_->base<T>() + root_->size() / sizeof(T);
    CHECK_LE(this->base<T>(), root_limit);
    CHECK_LE(this->base<T>() + n, root_limit);
    // Hold a ref of the underlying root buffer.
    // NOTE: 'buf' is a sub-buffer inside the 'root_' buffer.
    root_->Ref();
  }