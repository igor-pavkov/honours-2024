inline SegmentBuilder::SegmentBuilder(
    BuilderArena* arena, SegmentId id, const word* ptr, SegmentWordCount size,
    ReadLimiter* readLimiter)
    : SegmentReader(arena, id, ptr, size, readLimiter),
      // const_cast is safe here because the member won't ever be dereferenced because it appears
      // to point to the end of the segment anyway.
      pos(const_cast<word*>(ptr + size)), readOnly(true) {}