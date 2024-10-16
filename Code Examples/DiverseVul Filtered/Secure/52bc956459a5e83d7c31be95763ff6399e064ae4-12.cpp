inline SegmentBuilder::SegmentBuilder(
    BuilderArena* arena, SegmentId id, word* ptr, SegmentWordCount size,
    ReadLimiter* readLimiter, SegmentWordCount wordsUsed)
    : SegmentReader(arena, id, ptr, size, readLimiter),
      pos(ptr + wordsUsed), readOnly(false) {}