void Http2Ping::MemoryInfo(MemoryTracker* tracker) const {
  tracker->TrackField("callback", callback_);
}