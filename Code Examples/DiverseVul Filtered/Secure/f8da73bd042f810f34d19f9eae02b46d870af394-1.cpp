static int session_after_header_block_received(nghttp2_session *session) {
  int rv = 0;
  nghttp2_frame *frame = &session->iframe.frame;
  nghttp2_stream *stream;

  /* We don't call on_frame_recv_callback if stream has been closed
     already or being closed. */
  stream = nghttp2_session_get_stream(session, frame->hd.stream_id);
  if (!stream || stream->state == NGHTTP2_STREAM_CLOSING) {
    return 0;
  }

  if (session_enforce_http_messaging(session)) {
    if (frame->hd.type == NGHTTP2_PUSH_PROMISE) {
      nghttp2_stream *subject_stream;

      subject_stream = nghttp2_session_get_stream(
          session, frame->push_promise.promised_stream_id);
      if (subject_stream) {
        rv = nghttp2_http_on_request_headers(subject_stream, frame);
      }
    } else {
      assert(frame->hd.type == NGHTTP2_HEADERS);
      switch (frame->headers.cat) {
      case NGHTTP2_HCAT_REQUEST:
        rv = nghttp2_http_on_request_headers(stream, frame);
        break;
      case NGHTTP2_HCAT_RESPONSE:
      case NGHTTP2_HCAT_PUSH_RESPONSE:
        rv = nghttp2_http_on_response_headers(stream);
        break;
      case NGHTTP2_HCAT_HEADERS:
        if (stream->http_flags & NGHTTP2_HTTP_FLAG_EXPECT_FINAL_RESPONSE) {
          assert(!session->server);
          rv = nghttp2_http_on_response_headers(stream);
        } else {
          rv = nghttp2_http_on_trailer_headers(stream, frame);
        }
        break;
      default:
        assert(0);
      }
      if (rv == 0 && (frame->hd.flags & NGHTTP2_FLAG_END_STREAM)) {
        rv = nghttp2_http_on_remote_end_stream(stream);
      }
    }
    if (rv != 0) {
      int32_t stream_id;

      if (frame->hd.type == NGHTTP2_PUSH_PROMISE) {
        stream_id = frame->push_promise.promised_stream_id;
      } else {
        stream_id = frame->hd.stream_id;
      }

      rv = session_handle_invalid_stream2(session, stream_id, frame,
                                          NGHTTP2_ERR_HTTP_MESSAGING);
      if (nghttp2_is_fatal(rv)) {
        return rv;
      }

      if (frame->hd.type == NGHTTP2_HEADERS &&
          (frame->hd.flags & NGHTTP2_FLAG_END_STREAM)) {
        nghttp2_stream_shutdown(stream, NGHTTP2_SHUT_RD);
        /* Don't call nghttp2_session_close_stream_if_shut_rdwr
           because RST_STREAM has been submitted. */
      }
      return 0;
    }
  }

  rv = session_call_on_frame_received(session, frame);
  if (nghttp2_is_fatal(rv)) {
    return rv;
  }

  if (frame->hd.type != NGHTTP2_HEADERS) {
    return 0;
  }

  return session_end_stream_headers_received(session, frame, stream);
}