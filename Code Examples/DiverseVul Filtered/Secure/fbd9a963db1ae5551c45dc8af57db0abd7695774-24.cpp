MagickPrivate void ConvertHSBToRGB(const double hue,const double saturation,
  const double brightness,double *red,double *green,double *blue)
{
  double
    f,
    h,
    p,
    q,
    t;

  /*
    Convert HSB to RGB colorspace.
  */
  assert(red != (double *) NULL);
  assert(green != (double *) NULL);
  assert(blue != (double *) NULL);
  if (fabs(saturation) < MagickEpsilon)
    {
      *red=QuantumRange*brightness;
      *green=(*red);
      *blue=(*red);
      return;
    }
  h=6.0*(hue-floor(hue));
  f=h-floor((double) h);
  p=brightness*(1.0-saturation);
  q=brightness*(1.0-saturation*f);
  t=brightness*(1.0-(saturation*(1.0-f)));
  switch ((int) h)
  {
    case 0:
    default:
    {
      *red=QuantumRange*brightness;
      *green=QuantumRange*t;
      *blue=QuantumRange*p;
      break;
    }
    case 1:
    {
      *red=QuantumRange*q;
      *green=QuantumRange*brightness;
      *blue=QuantumRange*p;
      break;
    }
    case 2:
    {
      *red=QuantumRange*p;
      *green=QuantumRange*brightness;
      *blue=QuantumRange*t;
      break;
    }
    case 3:
    {
      *red=QuantumRange*p;
      *green=QuantumRange*q;
      *blue=QuantumRange*brightness;
      break;
    }
    case 4:
    {
      *red=QuantumRange*t;
      *green=QuantumRange*p;
      *blue=QuantumRange*brightness;
      break;
    }
    case 5:
    {
      *red=QuantumRange*brightness;
      *green=QuantumRange*p;
      *blue=QuantumRange*q;
      break;
    }
  }
}