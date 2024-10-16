inline SegmentReader::SegmentReader(Arena* arena, SegmentId id, const word* ptr,
                                    SegmentWordCount size, ReadLimiter* readLimiter)
    : arena(arena), id(id), ptr(kj::arrayPtr(ptr, unbound(size / WORDS))),
      readLimiter(readLimiter) {}