void Http2Session::StopTrackingRcbuf(nghttp2_rcbuf* buf) {
  StopTrackingMemory(buf);
}