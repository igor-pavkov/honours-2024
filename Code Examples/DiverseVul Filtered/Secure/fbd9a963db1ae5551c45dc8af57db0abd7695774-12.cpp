MagickPrivate void ConvertHWBToRGB(const double hue,const double whiteness,
  const double blackness,double *red,double *green,double *blue)
{
  double
    b,
    f,
    g,
    n,
    r,
    v;

  ssize_t
    i;

  /*
    Convert HWB to RGB colorspace.
  */
  assert(red != (double *) NULL);
  assert(green != (double *) NULL);
  assert(blue != (double *) NULL);
  v=1.0-blackness;
  if (fabs(hue-(-1.0)) < MagickEpsilon)
    {
      *red=QuantumRange*v;
      *green=QuantumRange*v;
      *blue=QuantumRange*v;
      return;
    }
  i=CastDoubleToLong(floor(6.0*hue));
  f=6.0*hue-i;
  if ((i & 0x01) != 0)
    f=1.0-f;
  n=whiteness+f*(v-whiteness);  /* linear interpolation */
  switch (i)
  {
    default:
    case 6:
    case 0: r=v; g=n; b=whiteness; break;
    case 1: r=n; g=v; b=whiteness; break;
    case 2: r=whiteness; g=v; b=n; break;
    case 3: r=whiteness; g=n; b=v; break;
    case 4: r=n; g=whiteness; b=v; break;
    case 5: r=v; g=whiteness; b=n; break;
  }
  *red=QuantumRange*r;
  *green=QuantumRange*g;
  *blue=QuantumRange*b;
}