inline word* SegmentBuilder::allocate(SegmentWordCount amount) {
  if (intervalLength(pos, ptr.end(), MAX_SEGMENT_WORDS) < amount) {
    // Not enough space in the segment for this allocation.
    return nullptr;
  } else {
    // Success.
    word* result = pos;
    pos = pos + amount;
    return result;
  }
}