static inline size_t GetPSDPacketSize(const Image *image)
{
  if (image->storage_class == PseudoClass)
    {
      if (image->colors > 256)
        return(2);
    }
  if (image->depth > 16)
    return(4);
  if (image->depth > 8)
    return(2);

  return(1);
}