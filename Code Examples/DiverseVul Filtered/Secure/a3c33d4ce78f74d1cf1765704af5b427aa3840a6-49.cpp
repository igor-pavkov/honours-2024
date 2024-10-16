bool Http2Session::HasWritesOnSocketForStream(Http2Stream* stream) {
  for (const NgHttp2StreamWrite& wr : outgoing_buffers_) {
    if (wr.req_wrap && WriteWrap::FromObject(wr.req_wrap)->stream() == stream)
      return true;
  }
  return false;
}