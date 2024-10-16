static int session_after_frame_sent1(nghttp2_session *session) {
  int rv;
  nghttp2_active_outbound_item *aob = &session->aob;
  nghttp2_outbound_item *item = aob->item;
  nghttp2_bufs *framebufs = &aob->framebufs;
  nghttp2_frame *frame;
  nghttp2_stream *stream;

  frame = &item->frame;

  if (frame->hd.type == NGHTTP2_DATA) {
    nghttp2_data_aux_data *aux_data;

    aux_data = &item->aux_data.data;

    stream = nghttp2_session_get_stream(session, frame->hd.stream_id);
    /* We update flow control window after a frame was completely
       sent. This is possible because we choose payload length not to
       exceed the window */
    session->remote_window_size -= (int32_t)frame->hd.length;
    if (stream) {
      stream->remote_window_size -= (int32_t)frame->hd.length;
    }

    if (stream && aux_data->eof) {
      rv = nghttp2_stream_detach_item(stream);
      if (nghttp2_is_fatal(rv)) {
        return rv;
      }

      /* Call on_frame_send_callback after
         nghttp2_stream_detach_item(), so that application can issue
         nghttp2_submit_data() in the callback. */
      if (session->callbacks.on_frame_send_callback) {
        rv = session_call_on_frame_send(session, frame);
        if (nghttp2_is_fatal(rv)) {
          return rv;
        }
      }

      if (frame->hd.flags & NGHTTP2_FLAG_END_STREAM) {
        int stream_closed;

        stream_closed =
            (stream->shut_flags & NGHTTP2_SHUT_RDWR) == NGHTTP2_SHUT_RDWR;

        nghttp2_stream_shutdown(stream, NGHTTP2_SHUT_WR);

        rv = nghttp2_session_close_stream_if_shut_rdwr(session, stream);
        if (nghttp2_is_fatal(rv)) {
          return rv;
        }
        /* stream may be NULL if it was closed */
        if (stream_closed) {
          stream = NULL;
        }
      }
      return 0;
    }

    if (session->callbacks.on_frame_send_callback) {
      rv = session_call_on_frame_send(session, frame);
      if (nghttp2_is_fatal(rv)) {
        return rv;
      }
    }

    return 0;
  }

  /* non-DATA frame */

  if (frame->hd.type == NGHTTP2_HEADERS ||
      frame->hd.type == NGHTTP2_PUSH_PROMISE) {
    if (nghttp2_bufs_next_present(framebufs)) {
      DEBUGF("send: CONTINUATION exists, just return\n");
      return 0;
    }
  }
  rv = session_call_on_frame_send(session, frame);
  if (nghttp2_is_fatal(rv)) {
    return rv;
  }
  switch (frame->hd.type) {
  case NGHTTP2_HEADERS: {
    nghttp2_headers_aux_data *aux_data;

    stream = nghttp2_session_get_stream(session, frame->hd.stream_id);
    if (!stream) {
      return 0;
    }

    switch (frame->headers.cat) {
    case NGHTTP2_HCAT_REQUEST: {
      stream->state = NGHTTP2_STREAM_OPENING;
      if (frame->hd.flags & NGHTTP2_FLAG_END_STREAM) {
        nghttp2_stream_shutdown(stream, NGHTTP2_SHUT_WR);
      }
      rv = nghttp2_session_close_stream_if_shut_rdwr(session, stream);
      if (nghttp2_is_fatal(rv)) {
        return rv;
      }
      /* We assume aux_data is a pointer to nghttp2_headers_aux_data */
      aux_data = &item->aux_data.headers;
      if (aux_data->data_prd.read_callback) {
        /* nghttp2_submit_data() makes a copy of aux_data->data_prd */
        rv = nghttp2_submit_data(session, NGHTTP2_FLAG_END_STREAM,
                                 frame->hd.stream_id, &aux_data->data_prd);
        if (nghttp2_is_fatal(rv)) {
          return rv;
        }
        /* TODO nghttp2_submit_data() may fail if stream has already
           DATA frame item.  We might have to handle it here. */
      }
      return 0;
    }
    case NGHTTP2_HCAT_PUSH_RESPONSE:
      stream->flags = (uint8_t)(stream->flags & ~NGHTTP2_STREAM_FLAG_PUSH);
      ++session->num_outgoing_streams;
    /* Fall through */
    case NGHTTP2_HCAT_RESPONSE:
      stream->state = NGHTTP2_STREAM_OPENED;
    /* Fall through */
    case NGHTTP2_HCAT_HEADERS:
      if (frame->hd.flags & NGHTTP2_FLAG_END_STREAM) {
        nghttp2_stream_shutdown(stream, NGHTTP2_SHUT_WR);
      }
      rv = nghttp2_session_close_stream_if_shut_rdwr(session, stream);
      if (nghttp2_is_fatal(rv)) {
        return rv;
      }
      /* We assume aux_data is a pointer to nghttp2_headers_aux_data */
      aux_data = &item->aux_data.headers;
      if (aux_data->data_prd.read_callback) {
        rv = nghttp2_submit_data(session, NGHTTP2_FLAG_END_STREAM,
                                 frame->hd.stream_id, &aux_data->data_prd);
        if (nghttp2_is_fatal(rv)) {
          return rv;
        }
        /* TODO nghttp2_submit_data() may fail if stream has already
           DATA frame item.  We might have to handle it here. */
      }
      return 0;
    default:
      /* Unreachable */
      assert(0);
      return 0;
    }
  }
  case NGHTTP2_PRIORITY:
    if (session->server) {
      return 0;
      ;
    }

    stream = nghttp2_session_get_stream_raw(session, frame->hd.stream_id);

    if (!stream) {
      if (!session_detect_idle_stream(session, frame->hd.stream_id)) {
        return 0;
      }

      stream = nghttp2_session_open_stream(
          session, frame->hd.stream_id, NGHTTP2_FLAG_NONE,
          &frame->priority.pri_spec, NGHTTP2_STREAM_IDLE, NULL);
      if (!stream) {
        return NGHTTP2_ERR_NOMEM;
      }
    } else {
      rv = nghttp2_session_reprioritize_stream(session, stream,
                                               &frame->priority.pri_spec);
      if (nghttp2_is_fatal(rv)) {
        return rv;
      }
    }

    rv = nghttp2_session_adjust_idle_stream(session);

    if (nghttp2_is_fatal(rv)) {
      return rv;
    }

    return 0;
  case NGHTTP2_RST_STREAM:
    rv = nghttp2_session_close_stream(session, frame->hd.stream_id,
                                      frame->rst_stream.error_code);
    if (nghttp2_is_fatal(rv)) {
      return rv;
    }
    return 0;
  case NGHTTP2_GOAWAY: {
    nghttp2_goaway_aux_data *aux_data;

    aux_data = &item->aux_data.goaway;

    if ((aux_data->flags & NGHTTP2_GOAWAY_AUX_SHUTDOWN_NOTICE) == 0) {

      if (aux_data->flags & NGHTTP2_GOAWAY_AUX_TERM_ON_SEND) {
        session->goaway_flags |= NGHTTP2_GOAWAY_TERM_SENT;
      }

      session->goaway_flags |= NGHTTP2_GOAWAY_SENT;

      rv = session_close_stream_on_goaway(session, frame->goaway.last_stream_id,
                                          1);

      if (nghttp2_is_fatal(rv)) {
        return rv;
      }
    }

    return 0;
  }
  case NGHTTP2_WINDOW_UPDATE:
    if (frame->hd.stream_id == 0) {
      session->window_update_queued = 0;
      if (session->opt_flags & NGHTTP2_OPTMASK_NO_AUTO_WINDOW_UPDATE) {
        rv = session_update_connection_consumed_size(session, 0);
      } else {
        rv = nghttp2_session_update_recv_connection_window_size(session, 0);
      }

      if (nghttp2_is_fatal(rv)) {
        return rv;
      }

      return 0;
    }

    stream = nghttp2_session_get_stream(session, frame->hd.stream_id);
    if (!stream) {
      return 0;
    }

    stream->window_update_queued = 0;

    /* We don't have to send WINDOW_UPDATE if END_STREAM from peer
       is seen. */
    if (stream->shut_flags & NGHTTP2_SHUT_RD) {
      return 0;
    }

    if (session->opt_flags & NGHTTP2_OPTMASK_NO_AUTO_WINDOW_UPDATE) {
      rv = session_update_stream_consumed_size(session, stream, 0);
    } else {
      rv =
          nghttp2_session_update_recv_stream_window_size(session, stream, 0, 1);
    }

    if (nghttp2_is_fatal(rv)) {
      return rv;
    }

    return 0;
  default:
    return 0;
  }
}