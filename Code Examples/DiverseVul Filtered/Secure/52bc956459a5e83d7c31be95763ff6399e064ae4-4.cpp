inline bool SegmentBuilder::tryExtend(word* from, word* to) {
  // Careful about overflow.
  if (pos == from && to <= ptr.end() && to >= from) {
    pos = to;
    return true;
  } else {
    return false;
  }
}