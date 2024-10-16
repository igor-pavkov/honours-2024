MagickPrivate void ConvertRGBToHSB(const double red,const double green,
  const double blue,double *hue,double *saturation,double *brightness)
{
  double
    delta,
    max,
    min;

  /*
    Convert RGB to HSB colorspace.
  */
  assert(hue != (double *) NULL);
  assert(saturation != (double *) NULL);
  assert(brightness != (double *) NULL);
  *hue=0.0;
  *saturation=0.0;
  *brightness=0.0;
  min=red < green ? red : green;
  if (blue < min)
    min=blue;
  max=red > green ? red : green;
  if (blue > max)
    max=blue;
  if (fabs(max) < MagickEpsilon)
    return;
  delta=max-min;
  *saturation=delta/max;
  *brightness=QuantumScale*max;
  if (fabs(delta) < MagickEpsilon)
    return;
  if (fabs(red-max) < MagickEpsilon)
    *hue=(green-blue)/delta;
  else
    if (fabs(green-max) < MagickEpsilon)
      *hue=2.0+(blue-red)/delta;
    else
      *hue=4.0+(red-green)/delta;
  *hue/=6.0;
  if (*hue < 0.0)
    *hue+=1.0;
}