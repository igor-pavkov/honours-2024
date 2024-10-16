void Http2Session::ClearOutgoing(int status) {
  CHECK(is_sending());

  set_sending(false);

  if (!outgoing_buffers_.empty()) {
    outgoing_storage_.clear();
    outgoing_length_ = 0;

    std::vector<NgHttp2StreamWrite> current_outgoing_buffers_;
    current_outgoing_buffers_.swap(outgoing_buffers_);
    for (const NgHttp2StreamWrite& wr : current_outgoing_buffers_) {
      BaseObjectPtr<AsyncWrap> wrap = std::move(wr.req_wrap);
      if (wrap) {
        // TODO(addaleax): Pass `status` instead of 0, so that we actually error
        // out with the error from the write to the underlying protocol,
        // if one occurred.
        WriteWrap::FromObject(wrap)->Done(0);
      }
    }
  }

  // Now that we've finished sending queued data, if there are any pending
  // RstStreams we should try sending again and then flush them one by one.
  if (!pending_rst_streams_.empty()) {
    std::vector<int32_t> current_pending_rst_streams;
    pending_rst_streams_.swap(current_pending_rst_streams);

    SendPendingData();

    for (int32_t stream_id : current_pending_rst_streams) {
      BaseObjectPtr<Http2Stream> stream = FindStream(stream_id);
      if (LIKELY(stream))
        stream->FlushRstStream();
    }
  }
}