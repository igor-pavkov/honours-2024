void Http2Stream::MemoryInfo(MemoryTracker* tracker) const {
  tracker->TrackField("current_headers", current_headers_);
  tracker->TrackField("queue", queue_);
}