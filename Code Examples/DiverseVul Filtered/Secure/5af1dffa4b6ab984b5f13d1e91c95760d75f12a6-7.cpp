static void ExportRGBQuantum(const Image *image,QuantumInfo *quantum_info,
  const MagickSizeType number_pixels,const Quantum *magick_restrict p,
  unsigned char *magick_restrict q,ExceptionInfo *exception)
{
  QuantumAny
    range;

  ssize_t
    x;

  ssize_t
    bit;

  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickCoreSignature);
  switch (quantum_info->depth)
  {
    case 8:
    {
      for (x=0; x < (ssize_t) number_pixels; x++)
      {
        q=PopCharPixel(ScaleQuantumToChar(GetPixelRed(image,p)),q);
        q=PopCharPixel(ScaleQuantumToChar(GetPixelGreen(image,p)),q);
        q=PopCharPixel(ScaleQuantumToChar(GetPixelBlue(image,p)),q);
        p+=GetPixelChannels(image);
        q+=quantum_info->pad;
      }
      break;
    }
    case 10:
    {
      unsigned int
        pixel;

      range=GetQuantumRange(quantum_info->depth);
      if (quantum_info->pack == MagickFalse)
        {
          for (x=0; x < (ssize_t) number_pixels; x++)
          {
            pixel=(unsigned int) (
              ScaleQuantumToAny(GetPixelRed(image,p),range) << 22 |
              ScaleQuantumToAny(GetPixelGreen(image,p),range) << 12 |
              ScaleQuantumToAny(GetPixelBlue(image,p),range) << 2);
            q=PopLongPixel(quantum_info->endian,pixel,q);
            p+=GetPixelChannels(image);
            q+=quantum_info->pad;
          }
          break;
        }
      if (quantum_info->quantum == 32UL)
        {
          for (x=0; x < (ssize_t) number_pixels; x++)
          {
            pixel=(unsigned int) ScaleQuantumToAny(GetPixelRed(image,p),range);
            q=PopQuantumLongPixel(quantum_info,pixel,q);
            pixel=(unsigned int) ScaleQuantumToAny(GetPixelGreen(image,p),
              range);
            q=PopQuantumLongPixel(quantum_info,pixel,q);
            pixel=(unsigned int) ScaleQuantumToAny(GetPixelBlue(image,p),range);
            q=PopQuantumLongPixel(quantum_info,pixel,q);
            p+=GetPixelChannels(image);
            q+=quantum_info->pad;
          }
          break;
        }
      for (x=0; x < (ssize_t) number_pixels; x++)
      {
        pixel=(unsigned int) ScaleQuantumToAny(GetPixelRed(image,p),range);
        q=PopQuantumPixel(quantum_info,pixel,q);
        pixel=(unsigned int) ScaleQuantumToAny(GetPixelGreen(image,p),range);
        q=PopQuantumPixel(quantum_info,pixel,q);
        pixel=(unsigned int) ScaleQuantumToAny(GetPixelBlue(image,p),range);
        q=PopQuantumPixel(quantum_info,pixel,q);
        p+=GetPixelChannels(image);
        q+=quantum_info->pad;
      }
      break;
    }
    case 12:
    {
      unsigned int
        pixel;

      range=GetQuantumRange(quantum_info->depth);
      if (quantum_info->pack == MagickFalse)
        {
          for (x=0; x < (ssize_t) (3*number_pixels-1); x+=2)
          {
            switch (x % 3)
            {
              default:
              case 0:
              {
                pixel=(unsigned int) ScaleQuantumToAny(GetPixelRed(image,p),
                  range);
                break;
              }
              case 1:
              {
                pixel=(unsigned int) ScaleQuantumToAny(GetPixelGreen(image,p),
                  range);
                break;
              }
              case 2:
              {
                pixel=(unsigned int) ScaleQuantumToAny(GetPixelBlue(image,p),
                  range);
                p+=GetPixelChannels(image);
                break;
              }
            }
            q=PopShortPixel(quantum_info->endian,(unsigned short) (pixel << 4),
              q);
            switch ((x+1) % 3)
            {
              default:
              case 0:
              {
                pixel=(unsigned int) ScaleQuantumToAny(GetPixelRed(image,p),
                  range);
                break;
              }
              case 1:
              {
                pixel=(unsigned int) ScaleQuantumToAny(GetPixelGreen(image,p),
                  range);
                break;
              }
              case 2:
              {
                pixel=(unsigned int) ScaleQuantumToAny(GetPixelBlue(image,p),
                  range);
                p+=GetPixelChannels(image);
                break;
              }
            }
            q=PopShortPixel(quantum_info->endian,(unsigned short) (pixel << 4),
              q);
            q+=quantum_info->pad;
          }
          for (bit=0; bit < (ssize_t) (3*number_pixels % 2); bit++)
          {
            switch ((x+bit) % 3)
            {
              default:
              case 0:
              {
                pixel=(unsigned int) ScaleQuantumToAny(GetPixelRed(image,p),
                  range);
                break;
              }
              case 1:
              {
                pixel=(unsigned int) ScaleQuantumToAny(GetPixelGreen(image,p),
                  range);
                break;
              }
              case 2:
              {
                pixel=(unsigned int) ScaleQuantumToAny(GetPixelBlue(image,p),
                  range);
                p+=GetPixelChannels(image);
                break;
              }
            }
            q=PopShortPixel(quantum_info->endian,(unsigned short) (pixel << 4),
              q);
            q+=quantum_info->pad;
          }
          if (bit != 0)
            p+=GetPixelChannels(image);
          break;
        }
      if (quantum_info->quantum == 32UL)
        {
          for (x=0; x < (ssize_t) number_pixels; x++)
          {
            pixel=(unsigned int) ScaleQuantumToAny(GetPixelRed(image,p),range);
            q=PopQuantumLongPixel(quantum_info,pixel,q);
            pixel=(unsigned int) ScaleQuantumToAny(GetPixelGreen(image,p),
              range);
            q=PopQuantumLongPixel(quantum_info,pixel,q);
            pixel=(unsigned int) ScaleQuantumToAny(GetPixelBlue(image,p),range);
            q=PopQuantumLongPixel(quantum_info,pixel,q);
            p+=GetPixelChannels(image);
            q+=quantum_info->pad;
          }
          break;
        }
      for (x=0; x < (ssize_t) number_pixels; x++)
      {
        pixel=(unsigned int) ScaleQuantumToAny(GetPixelRed(image,p),range);
        q=PopQuantumPixel(quantum_info,pixel,q);
        pixel=(unsigned int) ScaleQuantumToAny(GetPixelGreen(image,p),range);
        q=PopQuantumPixel(quantum_info,pixel,q);
        pixel=(unsigned int) ScaleQuantumToAny(GetPixelBlue(image,p),range);
        q=PopQuantumPixel(quantum_info,pixel,q);
        p+=GetPixelChannels(image);
        q+=quantum_info->pad;
      }
      break;
    }
    case 16:
    {
      unsigned short
        pixel;

      if (quantum_info->format == FloatingPointQuantumFormat)
        {
          for (x=0; x < (ssize_t) number_pixels; x++)
          {
            pixel=SinglePrecisionToHalf(QuantumScale*GetPixelRed(image,p));
            q=PopShortPixel(quantum_info->endian,pixel,q);
            pixel=SinglePrecisionToHalf(QuantumScale*GetPixelGreen(image,p));
            q=PopShortPixel(quantum_info->endian,pixel,q);
            pixel=SinglePrecisionToHalf(QuantumScale*GetPixelBlue(image,p));
            q=PopShortPixel(quantum_info->endian,pixel,q);
            p+=GetPixelChannels(image);
            q+=quantum_info->pad;
          }
          break;
        }
      for (x=0; x < (ssize_t) number_pixels; x++)
      {
        pixel=ScaleQuantumToShort(GetPixelRed(image,p));
        q=PopShortPixel(quantum_info->endian,pixel,q);
        pixel=ScaleQuantumToShort(GetPixelGreen(image,p));
        q=PopShortPixel(quantum_info->endian,pixel,q);
        pixel=ScaleQuantumToShort(GetPixelBlue(image,p));
        q=PopShortPixel(quantum_info->endian,pixel,q);
        p+=GetPixelChannels(image);
        q+=quantum_info->pad;
      }
      break;
    }
    case 32:
    {
      unsigned int
        pixel;

      if (quantum_info->format == FloatingPointQuantumFormat)
        {
          for (x=0; x < (ssize_t) number_pixels; x++)
          {
            q=PopFloatPixel(quantum_info,(float) GetPixelRed(image,p),q);
            q=PopFloatPixel(quantum_info,(float) GetPixelGreen(image,p),q);
            q=PopFloatPixel(quantum_info,(float) GetPixelBlue(image,p),q);
            p+=GetPixelChannels(image);
            q+=quantum_info->pad;
          }
          break;
        }
      for (x=0; x < (ssize_t) number_pixels; x++)
      {
        pixel=ScaleQuantumToLong(GetPixelRed(image,p));
        q=PopLongPixel(quantum_info->endian,pixel,q);
        pixel=ScaleQuantumToLong(GetPixelGreen(image,p));
        q=PopLongPixel(quantum_info->endian,pixel,q);
        pixel=ScaleQuantumToLong(GetPixelBlue(image,p));
        q=PopLongPixel(quantum_info->endian,pixel,q);
        p+=GetPixelChannels(image);
        q+=quantum_info->pad;
      }
      break;
    }
    case 64:
    {
      if (quantum_info->format == FloatingPointQuantumFormat)
        {
          for (x=0; x < (ssize_t) number_pixels; x++)
          {
            q=PopDoublePixel(quantum_info,(double) GetPixelRed(image,p),q);
            q=PopDoublePixel(quantum_info,(double) GetPixelGreen(image,p),q);
            q=PopDoublePixel(quantum_info,(double) GetPixelBlue(image,p),q);
            p+=GetPixelChannels(image);
            q+=quantum_info->pad;
          }
          break;
        }
    }
    default:
    {
      range=GetQuantumRange(quantum_info->depth);
      for (x=0; x < (ssize_t) number_pixels; x++)
      {
        q=PopQuantumPixel(quantum_info,ScaleQuantumToAny(GetPixelRed(image,p),
          range),q);
        q=PopQuantumPixel(quantum_info,ScaleQuantumToAny(GetPixelGreen(image,p),
          range),q);
        q=PopQuantumPixel(quantum_info,ScaleQuantumToAny(GetPixelBlue(image,p),
          range),q);
        p+=GetPixelChannels(image);
        q+=quantum_info->pad;
      }
      break;
    }
  }
}