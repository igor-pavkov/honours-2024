void NgHttp2StreamWrite::MemoryInfo(MemoryTracker* tracker) const {
  if (req_wrap)
    tracker->TrackField("req_wrap", req_wrap);
  tracker->TrackField("buf", buf);
}