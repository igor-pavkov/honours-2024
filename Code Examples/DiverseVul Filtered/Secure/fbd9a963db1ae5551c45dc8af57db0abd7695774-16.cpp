MagickPrivate void ConvertHSIToRGB(const double hue,const double saturation,
  const double intensity,double *red,double *green,double *blue)
{
  double
    b,
    g,
    h,
    r;

  /*
    Convert HSI to RGB colorspace.
  */
  assert(red != (double *) NULL);
  assert(green != (double *) NULL);
  assert(blue != (double *) NULL);
  h=360.0*hue;
  h-=360.0*floor(h/360.0);
  if (h < 120.0)
    {
      b=intensity*(1.0-saturation);
      r=intensity*(1.0+saturation*cos(h*(MagickPI/180.0))/cos((60.0-h)*
        (MagickPI/180.0)));
      g=3.0*intensity-r-b;
    }
  else
    if (h < 240.0)
      {
        h-=120.0;
        r=intensity*(1.0-saturation);
        g=intensity*(1.0+saturation*cos(h*(MagickPI/180.0))/cos((60.0-h)*
          (MagickPI/180.0)));
        b=3.0*intensity-r-g;
      }
    else
      {
        h-=240.0;
        g=intensity*(1.0-saturation);
        b=intensity*(1.0+saturation*cos(h*(MagickPI/180.0))/cos((60.0-h)*
          (MagickPI/180.0)));
        r=3.0*intensity-g-b;
      }
  *red=QuantumRange*r;
  *green=QuantumRange*g;
  *blue=QuantumRange*b;
}