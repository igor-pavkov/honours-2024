int nghttp2_session_update_recv_connection_window_size(nghttp2_session *session,
                                                       size_t delta_size) {
  int rv;
  rv = adjust_recv_window_size(&session->recv_window_size, delta_size,
                               session->local_window_size);
  if (rv != 0) {
    return nghttp2_session_terminate_session(session,
                                             NGHTTP2_FLOW_CONTROL_ERROR);
  }
  if (!(session->opt_flags & NGHTTP2_OPTMASK_NO_AUTO_WINDOW_UPDATE) &&
      session->window_update_queued == 0 &&
      nghttp2_should_send_window_update(session->local_window_size,
                                        session->recv_window_size)) {
    /* Use stream ID 0 to update connection-level flow control
       window */
    rv = nghttp2_session_add_window_update(session, NGHTTP2_FLAG_NONE, 0,
                                           session->recv_window_size);
    if (rv != 0) {
      return rv;
    }

    session->recv_window_size = 0;
  }
  return 0;
}