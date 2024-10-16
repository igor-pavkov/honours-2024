static inline size_t ColorToNodeId(const CubeInfo *cube_info,
  const DoublePixelPacket *pixel,size_t index)
{
  size_t
    id;

  id=(size_t) (((ScaleQuantumToChar(ClampPixel(pixel->red)) >> index) & 0x01) |
    ((ScaleQuantumToChar(ClampPixel(pixel->green)) >> index) & 0x01) << 1 |
    ((ScaleQuantumToChar(ClampPixel(pixel->blue)) >> index) & 0x01) << 2);
  if (cube_info->associate_alpha != MagickFalse)
    id|=((ScaleQuantumToChar(ClampPixel(pixel->alpha)) >> index) & 0x1) << 3;
  return(id);
}