int32_t GetFrameID(const nghttp2_frame* frame) {
  // If this is a push promise, we want to grab the id of the promised stream
  return (frame->hd.type == NGHTTP2_PUSH_PROMISE) ?
      frame->push_promise.promised_stream_id :
      frame->hd.stream_id;
}