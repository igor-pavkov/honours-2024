inline word* SegmentBuilder::getPtrUnchecked(SegmentWordCount offset) {
  return const_cast<word*>(ptr.begin() + offset);
}