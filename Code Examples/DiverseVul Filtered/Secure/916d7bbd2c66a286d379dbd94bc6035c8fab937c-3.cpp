static inline MagickBooleanType IsColorEqual(const Image *image,
  const Quantum *p, const PixelInfo *q)
{
  MagickRealType
    blue,
    green,
    red;

  red=(MagickRealType) GetPixelRed(image,p);
  green=(MagickRealType) GetPixelGreen(image,p);
  blue=(MagickRealType) GetPixelBlue(image,p);
  if ((AbsolutePixelValue(red-q->red) < MagickEpsilon) &&
      (AbsolutePixelValue(green-q->green) < MagickEpsilon) &&
      (AbsolutePixelValue(blue-q->blue) < MagickEpsilon))
    return(MagickTrue);
  return(MagickFalse);
}