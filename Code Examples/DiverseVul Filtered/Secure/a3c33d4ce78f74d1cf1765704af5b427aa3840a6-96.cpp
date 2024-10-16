ssize_t Http2Session::OnSelectPadding(nghttp2_session* handle,
                                      const nghttp2_frame* frame,
                                      size_t maxPayloadLen,
                                      void* user_data) {
  Http2Session* session = static_cast<Http2Session*>(user_data);
  ssize_t padding = frame->hd.length;

  switch (session->padding_strategy_) {
    case PADDING_STRATEGY_NONE:
      // Fall-through
      break;
    case PADDING_STRATEGY_MAX:
      padding = session->OnMaxFrameSizePadding(padding, maxPayloadLen);
      break;
    case PADDING_STRATEGY_ALIGNED:
      padding = session->OnDWordAlignedPadding(padding, maxPayloadLen);
      break;
  }
  return padding;
}