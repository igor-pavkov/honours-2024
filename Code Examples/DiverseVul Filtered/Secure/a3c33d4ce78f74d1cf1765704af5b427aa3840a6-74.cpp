void Http2Settings::MemoryInfo(MemoryTracker* tracker) const {
  tracker->TrackField("callback", callback_);
}