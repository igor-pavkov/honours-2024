inline SegmentWordCount SegmentReader::getOffsetTo(const word* ptr) {
  KJ_IREQUIRE(this->ptr.begin() <= ptr && ptr <= this->ptr.end());
  return intervalLength(this->ptr.begin(), ptr, MAX_SEGMENT_WORDS);
}