int Http2Session::OnHeaderCallback(nghttp2_session* handle,
                                   const nghttp2_frame* frame,
                                   nghttp2_rcbuf* name,
                                   nghttp2_rcbuf* value,
                                   uint8_t flags,
                                   void* user_data) {
  Http2Session* session = static_cast<Http2Session*>(user_data);
  int32_t id = GetFrameID(frame);
  BaseObjectPtr<Http2Stream> stream = session->FindStream(id);
  // If stream is null at this point, either something odd has happened
  // or the stream was closed locally while header processing was occurring.
  // either way, do not proceed and close the stream.
  if (UNLIKELY(!stream))
    return NGHTTP2_ERR_TEMPORAL_CALLBACK_FAILURE;

  // If the stream has already been destroyed, ignore.
  if (!stream->is_destroyed() && !stream->AddHeader(name, value, flags)) {
    // This will only happen if the connected peer sends us more
    // than the allowed number of header items at any given time
    stream->SubmitRstStream(NGHTTP2_ENHANCE_YOUR_CALM);
    return NGHTTP2_ERR_TEMPORAL_CALLBACK_FAILURE;
  }
  return 0;
}