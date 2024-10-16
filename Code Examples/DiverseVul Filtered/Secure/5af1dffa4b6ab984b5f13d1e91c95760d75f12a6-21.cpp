static void ExportRGBAQuantum(const Image *image,QuantumInfo *quantum_info,
  const MagickSizeType number_pixels,const Quantum *magick_restrict p,
  unsigned char *magick_restrict q,ExceptionInfo *exception)
{
  QuantumAny
    range;

  ssize_t
    x;

  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickCoreSignature);
  switch (quantum_info->depth)
  {
    case 8:
    {
      unsigned char
        pixel;

      for (x=0; x < (ssize_t) number_pixels; x++)
      {
        pixel=ScaleQuantumToChar(GetPixelRed(image,p));
        q=PopCharPixel(pixel,q);
        pixel=ScaleQuantumToChar(GetPixelGreen(image,p));
        q=PopCharPixel(pixel,q);
        pixel=ScaleQuantumToChar(GetPixelBlue(image,p));
        q=PopCharPixel(pixel,q);
        pixel=ScaleQuantumToChar(GetPixelAlpha(image,p));
        q=PopCharPixel(pixel,q);
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
          ssize_t
            i;

          size_t
            quantum;

          ssize_t
            n;

          n=0;
          quantum=0;
          pixel=0;
          for (x=0; x < (ssize_t) number_pixels; x++)
          {
            for (i=0; i < 4; i++)
            {
              switch (i)
              {
                case 0: quantum=GetPixelRed(image,p); break;
                case 1: quantum=GetPixelGreen(image,p); break;
                case 2: quantum=GetPixelBlue(image,p); break;
                case 3: quantum=GetPixelAlpha(image,p); break;
              }
              switch (n % 3)
              {
                case 0:
                {
                  pixel|=(size_t) (ScaleQuantumToAny((Quantum) quantum,
                    range) << 22);
                  break;
                }
                case 1:
                {
                  pixel|=(size_t) (ScaleQuantumToAny((Quantum) quantum,
                    range) << 12);
                  break;
                }
                case 2:
                {
                  pixel|=(size_t) (ScaleQuantumToAny((Quantum) quantum,
                    range) << 2);
                  q=PopLongPixel(quantum_info->endian,pixel,q);
                  pixel=0;
                  break;
                }
              }
              n++;
            }
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
            pixel=(unsigned int) ScaleQuantumToAny(GetPixelAlpha(image,p),
              range);
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
        pixel=(unsigned int) ScaleQuantumToAny(GetPixelAlpha(image,p),range);
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
            pixel=SinglePrecisionToHalf(QuantumScale*GetPixelAlpha(image,p));
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
        pixel=ScaleQuantumToShort(GetPixelAlpha(image,p));
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
            float
              float_pixel;

            q=PopFloatPixel(quantum_info,(float) GetPixelRed(image,p),q);
            q=PopFloatPixel(quantum_info,(float) GetPixelGreen(image,p),q);
            q=PopFloatPixel(quantum_info,(float) GetPixelBlue(image,p),q);
            float_pixel=(float) GetPixelAlpha(image,p);
            q=PopFloatPixel(quantum_info,float_pixel,q);
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
        pixel=ScaleQuantumToLong(GetPixelAlpha(image,p));
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
          double
            pixel;

          for (x=0; x < (ssize_t) number_pixels; x++)
          {
            q=PopDoublePixel(quantum_info,(double) GetPixelRed(image,p),q);
            q=PopDoublePixel(quantum_info,(double) GetPixelGreen(image,p),q);
            q=PopDoublePixel(quantum_info,(double) GetPixelBlue(image,p),q);
            pixel=(double) GetPixelAlpha(image,p);
            q=PopDoublePixel(quantum_info,pixel,q);
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
        q=PopQuantumPixel(quantum_info,ScaleQuantumToAny(GetPixelAlpha(image,p),
          range),q);
        p+=GetPixelChannels(image);
        q+=quantum_info->pad;
      }
      break;
    }
  }
}