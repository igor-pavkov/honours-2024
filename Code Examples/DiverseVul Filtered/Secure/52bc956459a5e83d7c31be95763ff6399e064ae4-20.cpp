inline void SegmentReader::unread(WordCount64 amount) { readLimiter->unread(amount); }