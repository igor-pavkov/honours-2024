uv_buf_t Http2Session::OnStreamAlloc(size_t suggested_size) {
  return AllocatedBuffer::AllocateManaged(env(), suggested_size).release();
}