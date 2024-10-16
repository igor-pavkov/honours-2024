static inline void AssociateAlphaPixel(const Image *image,
  const CubeInfo *cube_info,const Quantum *pixel,DoublePixelPacket *alpha_pixel)
{
  double
    alpha;

  if ((cube_info->associate_alpha == MagickFalse) ||
      (GetPixelAlpha(image,pixel) == OpaqueAlpha))
    {
      alpha_pixel->red=(double) GetPixelRed(image,pixel);
      alpha_pixel->green=(double) GetPixelGreen(image,pixel);
      alpha_pixel->blue=(double) GetPixelBlue(image,pixel);
      alpha_pixel->alpha=(double) GetPixelAlpha(image,pixel);
      return;
    }
  alpha=(double) (QuantumScale*GetPixelAlpha(image,pixel));
  alpha_pixel->red=alpha*GetPixelRed(image,pixel);
  alpha_pixel->green=alpha*GetPixelGreen(image,pixel);
  alpha_pixel->blue=alpha*GetPixelBlue(image,pixel);
  alpha_pixel->alpha=(double) GetPixelAlpha(image,pixel);
}