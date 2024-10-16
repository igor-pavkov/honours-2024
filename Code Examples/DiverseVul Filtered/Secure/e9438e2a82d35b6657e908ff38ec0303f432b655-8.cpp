static inline const unsigned char *ReadResourceLong(const unsigned char *p,
  size_t *quantum)
{
  *quantum=(size_t) (*p++ << 24);
  *quantum|=(size_t) (*p++ << 16);
  *quantum|=(size_t) (*p++ << 8);
  *quantum|=(size_t) (*p++ << 0);
  return(p);
}