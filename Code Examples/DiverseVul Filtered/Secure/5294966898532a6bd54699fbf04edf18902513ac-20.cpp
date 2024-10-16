static inline void AssociateAlphaPixelInfo(const CubeInfo *cube_info,
  const PixelInfo *pixel,DoublePixelPacket *alpha_pixel)
{
  double
    alpha;

  if ((cube_info->associate_alpha == MagickFalse) ||
      (pixel->alpha == OpaqueAlpha))
    {
      alpha_pixel->red=(double) pixel->red;
      alpha_pixel->green=(double) pixel->green;
      alpha_pixel->blue=(double) pixel->blue;
      alpha_pixel->alpha=(double) pixel->alpha;
      return;
    }
  alpha=(double) (QuantumScale*pixel->alpha);
  alpha_pixel->red=alpha*pixel->red;
  alpha_pixel->green=alpha*pixel->green;
  alpha_pixel->blue=alpha*pixel->blue;
  alpha_pixel->alpha=(double) pixel->alpha;
}