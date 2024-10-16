inline void SegmentBuilder::tryTruncate(word* from, word* to) {
  if (pos == from) pos = to;
}