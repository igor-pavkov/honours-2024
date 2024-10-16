static void ExportIndexAlphaQuantum(const Image *image,
  QuantumInfo *quantum_info,const MagickSizeType number_pixels,
  const Quantum *magick_restrict p,unsigned char *magick_restrict q,
  ExceptionInfo *exception)
{
  ssize_t
    x;

  ssize_t
    bit;

  if (image->storage_class != PseudoClass)
    {
      (void) ThrowMagickException(exception,GetMagickModule(),ImageError,
        "ColormappedImageRequired","`%s'",image->filename);
      return;
    }
  switch (quantum_info->depth)
  {
    case 1:
    {
      unsigned char
        pixel;

      for (x=((ssize_t) number_pixels-3); x > 0; x-=4)
      {
        pixel=(unsigned char) GetPixelIndex(image,p);
        *q=((pixel & 0x01) << 7);
        pixel=(unsigned char) (GetPixelAlpha(image,p) == (Quantum)
          TransparentAlpha ? 1 : 0);
        *q|=((pixel & 0x01) << 6);
        p+=GetPixelChannels(image);
        pixel=(unsigned char) GetPixelIndex(image,p);
        *q|=((pixel & 0x01) << 5);
        pixel=(unsigned char) (GetPixelAlpha(image,p) == (Quantum)
          TransparentAlpha ? 1 : 0);
        *q|=((pixel & 0x01) << 4);
        p+=GetPixelChannels(image);
        pixel=(unsigned char) GetPixelIndex(image,p);
        *q|=((pixel & 0x01) << 3);
        pixel=(unsigned char) (GetPixelAlpha(image,p) == (Quantum)
          TransparentAlpha ? 1 : 0);
        *q|=((pixel & 0x01) << 2);
        p+=GetPixelChannels(image);
        pixel=(unsigned char) GetPixelIndex(image,p);
        *q|=((pixel & 0x01) << 1);
        pixel=(unsigned char) (GetPixelAlpha(image,p) == (Quantum)
          TransparentAlpha ? 1 : 0);
        *q|=((pixel & 0x01) << 0);
        p+=GetPixelChannels(image);
        q++;
      }
      if ((number_pixels % 4) != 0)
        {
          *q='\0';
          for (bit=3; bit >= (ssize_t) (4-(number_pixels % 4)); bit-=2)
          {
            pixel=(unsigned char) GetPixelIndex(image,p);
            *q|=((pixel & 0x01) << (unsigned char) (bit+4));
            pixel=(unsigned char) (GetPixelAlpha(image,p) == (Quantum)
              TransparentAlpha ? 1 : 0);
            *q|=((pixel & 0x01) << (unsigned char) (bit+4-1));
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

      for (x=0; x < (ssize_t) number_pixels ; x++)
      {
        pixel=(unsigned char) GetPixelIndex(image,p);
        *q=((pixel & 0xf) << 4);
        pixel=(unsigned char) (16*QuantumScale*GetPixelAlpha(image,p)+0.5);
        *q|=((pixel & 0xf) << 0);
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
        q=PopCharPixel((unsigned char) GetPixelIndex(image,p),q);
        pixel=ScaleQuantumToChar(GetPixelAlpha(image,p));
        q=PopCharPixel(pixel,q);
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
            q=PopShortPixel(quantum_info->endian,(unsigned short)
              GetPixelIndex(image,p),q);
            pixel=SinglePrecisionToHalf(QuantumScale*GetPixelAlpha(image,p));
            q=PopShortPixel(quantum_info->endian,pixel,q);
            p+=GetPixelChannels(image);
            q+=quantum_info->pad;
          }
          break;
        }
      for (x=0; x < (ssize_t) number_pixels; x++)
      {
        q=PopShortPixel(quantum_info->endian,(unsigned short)
              GetPixelIndex(image,p),q);
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

            q=PopFloatPixel(quantum_info,(float) GetPixelIndex(image,p),q);
            float_pixel=(float)  GetPixelAlpha(image,p);
            q=PopFloatPixel(quantum_info,float_pixel,q);
            p+=GetPixelChannels(image);
            q+=quantum_info->pad;
          }
          break;
        }
      for (x=0; x < (ssize_t) number_pixels; x++)
      {
        q=PopLongPixel(quantum_info->endian,(unsigned int)
          GetPixelIndex(image,p),q);
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
          for (x=0; x < (ssize_t) number_pixels; x++)
          {
            double
              pixel;

            q=PopDoublePixel(quantum_info,(double) GetPixelIndex(image,p),q);
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
      QuantumAny
        range;

      range=GetQuantumRange(quantum_info->depth);
      for (x=0; x < (ssize_t) number_pixels; x++)
      {
        q=PopQuantumPixel(quantum_info,GetPixelIndex(image,p),q);
        q=PopQuantumPixel(quantum_info,ScaleQuantumToAny(GetPixelAlpha(image,p),
          range),q);
        p+=GetPixelChannels(image);
        q+=quantum_info->pad;
      }
      break;
    }
  }
}