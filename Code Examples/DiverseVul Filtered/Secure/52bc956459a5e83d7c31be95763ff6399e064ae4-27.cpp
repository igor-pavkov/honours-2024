inline void SegmentBuilder::reset() {
  word* start = getPtrUnchecked(ZERO * WORDS);
  memset(start, 0, (pos - start) * sizeof(word));
  pos = start;
}