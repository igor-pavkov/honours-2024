static inline size_t ReadProfileLong(const EndianType endian,
  unsigned char *buffer)
{
  size_t
    value;

  if (endian == LSBEndian)
    {
      value=(size_t) ((buffer[3] << 24) | (buffer[2] << 16) |
        (buffer[1] << 8 ) | (buffer[0]));
      return((size_t) (value & 0xffffffff));
    }
  value=(size_t) ((buffer[0] << 24) | (buffer[1] << 16) |
    (buffer[2] << 8) | buffer[3]);
  return((size_t) (value & 0xffffffff));
}