void Http2Session::PushOutgoingBuffer(NgHttp2StreamWrite&& write) {
  outgoing_length_ += write.buf.len;
  outgoing_buffers_.emplace_back(std::move(write));
}