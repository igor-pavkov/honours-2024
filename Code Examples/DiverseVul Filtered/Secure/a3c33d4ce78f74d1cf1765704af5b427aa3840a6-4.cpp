void Http2Session::MemoryInfo(MemoryTracker* tracker) const {
  tracker->TrackField("streams", streams_);
  tracker->TrackField("outstanding_pings", outstanding_pings_);
  tracker->TrackField("outstanding_settings", outstanding_settings_);
  tracker->TrackField("outgoing_buffers", outgoing_buffers_);
  tracker->TrackFieldWithSize("stream_buf", stream_buf_.len);
  tracker->TrackFieldWithSize("outgoing_storage", outgoing_storage_.size());
  tracker->TrackFieldWithSize("pending_rst_streams",
                              pending_rst_streams_.size() * sizeof(int32_t));
  tracker->TrackFieldWithSize("nghttp2_memory", current_nghttp2_memory_);
}