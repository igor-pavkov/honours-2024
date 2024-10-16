static inline int PixelsEqual(const Quantum *source1,ssize_t offset1,
  const Quantum *source2,ssize_t offset2,const size_t channels)
{
  register ssize_t
    i;

  offset1*=channels;
  offset2*=channels;
  for (i=0; i < (ssize_t) channels; i++)
    if (source1[offset1+i] != source2[offset2+i])
      return(0);
  return(1);
}