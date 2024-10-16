static inline const unsigned char *ReadResourceBytes(const unsigned char *p,
  const ssize_t count,unsigned char *quantum)
{
  register ssize_t
    i;

  for (i=0; i < count; i++)
    *quantum++=(*p++);
  return(p);
}