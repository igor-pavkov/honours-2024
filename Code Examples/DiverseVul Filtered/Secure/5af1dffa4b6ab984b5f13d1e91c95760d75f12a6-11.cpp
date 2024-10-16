static void ExportGrayQuantum(const Image *image,QuantumInfo *quantum_info,
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
    case 1:
    {
      double
        threshold;

      unsigned char
        black,
        white;

      ssize_t
        bit;

      black=0x00;
      white=0x01;
      if (quantum_info->min_is_white != MagickFalse)
        {
          black=0x01;
          white=0x00;
        }
      threshold=QuantumRange/2.0;
      for (x=((ssize_t) number_pixels-7); x > 0; x-=8)
      {
        *q='\0';
        *q|=(GetPixelLuma(image,p) < threshold ? black : white) << 7;
        p+=GetPixelChannels(image);
        *q|=(GetPixelLuma(image,p) < threshold ? black : white) << 6;
        p+=GetPixelChannels(image);
        *q|=(GetPixelLuma(image,p) < threshold ? black : white) << 5;
        p+=GetPixelChannels(image);
        *q|=(GetPixelLuma(image,p) < threshold ? black : white) << 4;
        p+=GetPixelChannels(image);
        *q|=(GetPixelLuma(image,p) < threshold ? black : white) << 3;
        p+=GetPixelChannels(image);
        *q|=(GetPixelLuma(image,p) < threshold ? black : white) << 2;
        p+=GetPixelChannels(image);
        *q|=(GetPixelLuma(image,p) < threshold ? black : white) << 1;
        p+=GetPixelChannels(image);
        *q|=(GetPixelLuma(image,p) < threshold ? black : white) << 0;
        p+=GetPixelChannels(image);
        q++;
      }
      if ((number_pixels % 8) != 0)
        {
          *q='\0';
          for (bit=7; bit >= (ssize_t) (8-(number_pixels % 8)); bit--)
          {
            *q|=(GetPixelLuma(image,p) < threshold ? black : white) << bit;
            p+=GetPixelChannels(image);
          }
          q++;
        }
      break;
    }
    case 4:
    {
      unsigned char
        pixel;

      for (x=0; x < (ssize_t) (number_pixels-1) ; x+=2)
      {
        pixel=ScaleQuantumToChar(ClampToQuantum(GetPixelLuma(image,p)));
        *q=(((pixel >> 4) & 0xf) << 4);
        p+=GetPixelChannels(image);
        pixel=ScaleQuantumToChar(ClampToQuantum(GetPixelLuma(image,p)));
        *q|=pixel >> 4;
        p+=GetPixelChannels(image);
        q++;
      }
      if ((number_pixels % 2) != 0)
        {
          pixel=ScaleQuantumToChar(ClampToQuantum(GetPixelLuma(image,p)));
          *q=(((pixel >> 4) & 0xf) << 4);
          p+=GetPixelChannels(image);
          q++;
        }
      break;
    }
    case 8:
    {
      unsigned char
        pixel;

      for (x=0; x < (ssize_t) number_pixels; x++)
      {
        pixel=ScaleQuantumToChar(ClampToQuantum(GetPixelLuma(image,p)));
        q=PopCharPixel(pixel,q);
        p+=GetPixelChannels(image);
        q+=quantum_info->pad;
      }
      break;
    }
    case 10:
    {
      range=GetQuantumRange(quantum_info->depth);
      if (quantum_info->pack == MagickFalse)
        {
          unsigned int
            pixel;

          for (x=0; x < (ssize_t) (number_pixels-2); x+=3)
          {
            pixel=(unsigned int) (ScaleQuantumToAny(ClampToQuantum(
              GetPixelLuma(image,p+2*GetPixelChannels(image))),range) << 22 |
              ScaleQuantumToAny(ClampToQuantum(GetPixelLuma(image,p+
              GetPixelChannels(image))),range) << 12 | ScaleQuantumToAny(
              ClampToQuantum(GetPixelLuma(image,p)),range) << 2);
            q=PopLongPixel(quantum_info->endian,pixel,q);
            p+=3*GetPixelChannels(image);
            q+=quantum_info->pad;
          }
          if (x < (ssize_t) number_pixels)
            {
              pixel=0U;
              if (x++ < (ssize_t) (number_pixels-1))
                pixel|=ScaleQuantumToAny(ClampToQuantum(GetPixelLuma(image,p+
                  GetPixelChannels(image))),range) << 12;
              if (x++ < (ssize_t) number_pixels)
                pixel|=ScaleQuantumToAny(ClampToQuantum(GetPixelLuma(image,p)),
                  range) << 2;
              q=PopLongPixel(quantum_info->endian,pixel,q);
            }
          break;
        }
      for (x=0; x < (ssize_t) number_pixels; x++)
      {
        q=PopQuantumPixel(quantum_info,ScaleQuantumToAny(ClampToQuantum(
          GetPixelLuma(image,p)),range),q);
        p+=GetPixelChannels(image);
        q+=quantum_info->pad;
      }
      break;
    }
    case 12:
    {
      unsigned short
        pixel;

      range=GetQuantumRange(quantum_info->depth);
      if (quantum_info->pack == MagickFalse)
        {
          for (x=0; x < (ssize_t) number_pixels; x++)
          {
            pixel=ScaleQuantumToShort(ClampToQuantum(GetPixelLuma(image,p)));
            q=PopShortPixel(quantum_info->endian,(unsigned short) (pixel >> 4),
              q);
            p+=GetPixelChannels(image);
            q+=quantum_info->pad;
          }
          break;
        }
      for (x=0; x < (ssize_t) number_pixels; x++)
      {
        q=PopQuantumPixel(quantum_info,ScaleQuantumToAny(ClampToQuantum(
          GetPixelLuma(image,p)),range),q);
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
            pixel=SinglePrecisionToHalf(QuantumScale*GetPixelLuma(image,p));
            q=PopShortPixel(quantum_info->endian,pixel,q);
            p+=GetPixelChannels(image);
            q+=quantum_info->pad;
          }
          break;
        }
      for (x=0; x < (ssize_t) number_pixels; x++)
      {
        pixel=ScaleQuantumToShort(ClampToQuantum(GetPixelLuma(image,p)));
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

            float_pixel=(float) GetPixelLuma(image,p);
            q=PopFloatPixel(quantum_info,float_pixel,q);
            p+=GetPixelChannels(image);
            q+=quantum_info->pad;
          }
          break;
        }
      for (x=0; x < (ssize_t) number_pixels; x++)
      {
        pixel=ScaleQuantumToLong(ClampToQuantum(GetPixelLuma(image,p)));
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
            double
              pixel;

            pixel=GetPixelLuma(image,p);
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
        q=PopQuantumPixel(quantum_info,ScaleQuantumToAny(ClampToQuantum(
          GetPixelLuma(image,p)),range),q);
        p+=GetPixelChannels(image);
        q+=quantum_info->pad;
      }
      break;
    }
  }
}