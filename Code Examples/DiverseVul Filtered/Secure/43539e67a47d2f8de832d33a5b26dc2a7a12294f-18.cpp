static inline void CopyPixels(const Quantum *source,const ssize_t source_offset,
  Quantum *destination,const ssize_t destination_offset,const size_t channels)
{
  register ssize_t
    i;

  for (i=0; i < (ssize_t) channels; i++)
    destination[channels*destination_offset+i]=source[source_offset*channels+i];
}