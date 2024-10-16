MagickExport void ConvertHSLToRGB(const double hue,const double saturation,
  const double lightness,double *red,double *green,double *blue)
{
  double
    c,
    h,
    min,
    x;

  /*
    Convert HSL to RGB colorspace.
  */
  assert(red != (double *) NULL);
  assert(green != (double *) NULL);
  assert(blue != (double *) NULL);
  h=hue*360.0;
  if (lightness <= 0.5)
    c=2.0*lightness*saturation;
  else
    c=(2.0-2.0*lightness)*saturation;
  min=lightness-0.5*c;
  h-=360.0*floor(h/360.0);
  h/=60.0;
  x=c*(1.0-fabs(h-2.0*floor(h/2.0)-1.0));
  switch ((int) floor(h))
  {
    case 0:
    {
      *red=QuantumRange*(min+c);
      *green=QuantumRange*(min+x);
      *blue=QuantumRange*min;
      break;
    }
    case 1:
    {
      *red=QuantumRange*(min+x);
      *green=QuantumRange*(min+c);
      *blue=QuantumRange*min;
      break;
    }
    case 2:
    {
      *red=QuantumRange*min;
      *green=QuantumRange*(min+c);
      *blue=QuantumRange*(min+x);
      break;
    }
    case 3:
    {
      *red=QuantumRange*min;
      *green=QuantumRange*(min+x);
      *blue=QuantumRange*(min+c);
      break;
    }
    case 4:
    {
      *red=QuantumRange*(min+x);
      *green=QuantumRange*min;
      *blue=QuantumRange*(min+c);
      break;
    }
    case 5:
    {
      *red=QuantumRange*(min+c);
      *green=QuantumRange*min;
      *blue=QuantumRange*(min+x);
      break;
    }
    default:
    {
      *red=0.0;
      *green=0.0;
      *blue=0.0;
    }
  }
}