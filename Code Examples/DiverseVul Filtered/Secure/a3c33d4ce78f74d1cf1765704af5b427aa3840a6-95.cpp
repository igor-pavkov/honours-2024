Http2Session::Callbacks::Callbacks(bool kHasGetPaddingCallback) {
  nghttp2_session_callbacks* callbacks_;
  CHECK_EQ(nghttp2_session_callbacks_new(&callbacks_), 0);
  callbacks.reset(callbacks_);

  nghttp2_session_callbacks_set_on_begin_headers_callback(
    callbacks_, OnBeginHeadersCallback);
  nghttp2_session_callbacks_set_on_header_callback2(
    callbacks_, OnHeaderCallback);
  nghttp2_session_callbacks_set_on_frame_recv_callback(
    callbacks_, OnFrameReceive);
  nghttp2_session_callbacks_set_on_stream_close_callback(
    callbacks_, OnStreamClose);
  nghttp2_session_callbacks_set_on_data_chunk_recv_callback(
    callbacks_, OnDataChunkReceived);
  nghttp2_session_callbacks_set_on_frame_not_send_callback(
    callbacks_, OnFrameNotSent);
  nghttp2_session_callbacks_set_on_invalid_header_callback2(
    callbacks_, OnInvalidHeader);
  nghttp2_session_callbacks_set_error_callback(
    callbacks_, OnNghttpError);
  nghttp2_session_callbacks_set_send_data_callback(
    callbacks_, OnSendData);
  nghttp2_session_callbacks_set_on_invalid_frame_recv_callback(
    callbacks_, OnInvalidFrame);
  nghttp2_session_callbacks_set_on_frame_send_callback(
    callbacks_, OnFrameSent);

  if (kHasGetPaddingCallback) {
    nghttp2_session_callbacks_set_select_padding_callback(
      callbacks_, OnSelectPadding);
  }
}