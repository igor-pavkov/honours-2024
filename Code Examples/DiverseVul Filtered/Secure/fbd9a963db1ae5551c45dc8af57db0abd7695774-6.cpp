MagickPrivate void ConvertLCHuvToRGB(const double luma,const double chroma,
  const double hue,double *red,double *green,double *blue)
{
  double
    X,
    Y,
    Z;

  /*
    Convert LCHuv to RGB colorspace.
  */
  assert(red != (double *) NULL);
  assert(green != (double *) NULL);
  assert(blue != (double *) NULL);
  ConvertLCHuvToXYZ(100.0*luma,255.0*(chroma-0.5),360.0*hue,&X,&Y,&Z);
  ConvertXYZToRGB(X,Y,Z,red,green,blue);
}