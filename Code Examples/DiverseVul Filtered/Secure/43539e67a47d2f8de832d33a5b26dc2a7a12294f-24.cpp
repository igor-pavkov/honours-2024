static inline void MixPixels(const Quantum *source,const ssize_t *source_offset,
  const size_t source_size,Quantum *destination,
  const ssize_t destination_offset,const size_t channels)
{
  ssize_t
    sum;

  register ssize_t
    i;

  for (i=0; i < (ssize_t) channels; i++)
  {
    register ssize_t
      j;

    sum=0;
    for (j=0; j < (ssize_t) source_size; j++)
      sum+=source[source_offset[j]*channels+i];
    destination[channels*destination_offset+i]=(Quantum) (sum/source_size);
  }
}