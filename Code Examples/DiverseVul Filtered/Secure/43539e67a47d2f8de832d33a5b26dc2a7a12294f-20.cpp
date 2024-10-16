static inline void Mix2Pixels(const Quantum *source,
  const ssize_t source_offset1,const ssize_t source_offset2,
  Quantum *destination,const ssize_t destination_offset,const size_t channels)
{
  const ssize_t
    offsets[2] = { source_offset1, source_offset2 };

  MixPixels(source,offsets,2,destination,destination_offset,channels);
}