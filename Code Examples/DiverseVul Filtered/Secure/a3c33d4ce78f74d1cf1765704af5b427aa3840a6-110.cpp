void Http2Session::CopyDataIntoOutgoing(const uint8_t* src, size_t src_length) {
  size_t offset = outgoing_storage_.size();
  outgoing_storage_.resize(offset + src_length);
  memcpy(&outgoing_storage_[offset], src, src_length);

  // Store with a base of `nullptr` initially, since future resizes
  // of the outgoing_buffers_ vector may invalidate the pointer.
  // The correct base pointers will be set later, before writing to the
  // underlying socket.
  PushOutgoingBuffer(NgHttp2StreamWrite {
    uv_buf_init(nullptr, src_length)
  });
}