int Http2Session::HandleDataFrame(const nghttp2_frame* frame) {
  int32_t id = GetFrameID(frame);
  Debug(this, "handling data frame for stream %d", id);
  BaseObjectPtr<Http2Stream> stream = FindStream(id);

  if (stream &&
      !stream->is_destroyed() &&
      frame->hd.flags & NGHTTP2_FLAG_END_STREAM) {
    stream->EmitRead(UV_EOF);
  } else if (frame->hd.length == 0) {
    if (invalid_frame_count_++ > js_fields_->max_invalid_frames) {
      custom_recv_error_code_ = "ERR_HTTP2_TOO_MANY_INVALID_FRAMES";
      Debug(this, "rejecting empty-frame-without-END_STREAM flood\n");
      // Consider a flood of 0-length frames without END_STREAM an error.
      return 1;
    }
  }
  return 0;
}