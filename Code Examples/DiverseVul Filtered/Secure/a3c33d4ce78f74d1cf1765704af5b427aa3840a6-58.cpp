void Http2State::MemoryInfo(MemoryTracker* tracker) const {
  tracker->TrackField("root_buffer", root_buffer);
}