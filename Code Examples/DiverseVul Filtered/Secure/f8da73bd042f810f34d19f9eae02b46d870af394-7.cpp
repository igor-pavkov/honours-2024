int nghttp2_session_update_recv_stream_window_size(nghttp2_session *session,
                                                   nghttp2_stream *stream,
                                                   size_t delta_size,
                                                   int send_window_update) {
  int rv;
  rv = adjust_recv_window_size(&stream->recv_window_size, delta_size,
                               stream->local_window_size);
  if (rv != 0) {
    return nghttp2_session_add_rst_stream(session, stream->stream_id,
                                          NGHTTP2_FLOW_CONTROL_ERROR);
  }
  /* We don't have to send WINDOW_UPDATE if the data received is the
     last chunk in the incoming stream. */
  /* We have to use local_settings here because it is the constraint
     the remote endpoint should honor. */
  if (send_window_update &&
      !(session->opt_flags & NGHTTP2_OPTMASK_NO_AUTO_WINDOW_UPDATE) &&
      stream->window_update_queued == 0 &&
      nghttp2_should_send_window_update(stream->local_window_size,
                                        stream->recv_window_size)) {
    rv = nghttp2_session_add_window_update(session, NGHTTP2_FLAG_NONE,
                                           stream->stream_id,
                                           stream->recv_window_size);
    if (rv != 0) {
      return rv;
    }

    stream->recv_window_size = 0;
  }
  return 0;
}