inline SegmentWordCount SegmentReader::getSize() {
  return assumeBits<SEGMENT_WORD_COUNT_BITS>(ptr.size()) * WORDS;
}