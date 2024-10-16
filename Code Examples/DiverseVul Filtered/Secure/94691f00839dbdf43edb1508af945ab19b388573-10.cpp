static inline size_t ColorToNodeId(const Image *image,
  const PixelInfo *pixel,size_t index)
{
  size_t
    id;

  id=(size_t) (
    ((ScaleQuantumToChar(ClampToQuantum(pixel->red)) >> index) & 0x01) |
    ((ScaleQuantumToChar(ClampToQuantum(pixel->green)) >> index) & 0x01) << 1 |
    ((ScaleQuantumToChar(ClampToQuantum(pixel->blue)) >> index) & 0x01) << 2);
  if (image->alpha_trait != UndefinedPixelTrait)
    id|=((ScaleQuantumToChar(ClampToQuantum(pixel->alpha)) >> index) &
      0x01) << 3;
  return(id);
}