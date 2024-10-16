MagickPrivate void ConvertRGBToHCLp(const double red,const double green,
  const double blue,double *hue,double *chroma,double *luma)
{
  double
    c,
    h,
    max;

  /*
    Convert RGB to HCL colorspace.
  */
  assert(hue != (double *) NULL);
  assert(chroma != (double *) NULL);
  assert(luma != (double *) NULL);
  max=MagickMax(red,MagickMax(green,blue));
  c=max-MagickMin(red,MagickMin(green,blue));
  h=0.0;
  if (fabs(c) < MagickEpsilon)
    h=0.0;
  else
    if (fabs(red-max) < MagickEpsilon)
      h=fmod((green-blue)/c+6.0,6.0);
    else
      if (fabs(green-max) < MagickEpsilon)
        h=((blue-red)/c)+2.0;
      else
        if (fabs(blue-max) < MagickEpsilon)
          h=((red-green)/c)+4.0;
  *hue=(h/6.0);
  *chroma=QuantumScale*c;
  *luma=QuantumScale*(0.298839*red+0.586811*green+0.114350*blue);
}