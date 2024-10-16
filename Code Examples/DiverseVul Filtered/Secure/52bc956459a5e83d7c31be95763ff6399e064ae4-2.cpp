inline SegmentBuilder::SegmentBuilder(BuilderArena* arena, SegmentId id, decltype(nullptr),
                                      ReadLimiter* readLimiter)
    : SegmentReader(arena, id, nullptr, ZERO * WORDS, readLimiter),
      pos(nullptr), readOnly(false) {}