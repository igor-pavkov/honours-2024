static void ExportCMYKAQuantum(const Image *image,QuantumInfo *quantum_info,
  const MagickSizeType number_pixels,const Quantum *magick_restrict p,
  unsigned char *magick_restrict q,ExceptionInfo *exception)
{
  ssize_t
    x;

  if (image->colorspace != CMYKColorspace)
    {
      (void) ThrowMagickException(exception,GetMagickModule(),ImageError,
        "ColorSeparatedImageRequired","`%s'",image->filename);
      return;
    }
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
        pixel=ScaleQuantumToChar(GetPixelBlack(image,p));
        q=PopCharPixel(pixel,q);
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
            pixel=SinglePrecisionToHalf(QuantumScale*GetPixelRed(image,p));
            q=PopShortPixel(quantum_info->endian,pixel,q);
            pixel=SinglePrecisionToHalf(QuantumScale*GetPixelGreen(image,p));
            q=PopShortPixel(quantum_info->endian,pixel,q);
            pixel=SinglePrecisionToHalf(QuantumScale*GetPixelBlue(image,p));
            q=PopShortPixel(quantum_info->endian,pixel,q);
            pixel=SinglePrecisionToHalf(QuantumScale*GetPixelBlack(image,p));
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
        pixel=ScaleQuantumToShort(GetPixelBlack(image,p));
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
            q=PopFloatPixel(quantum_info,(float) GetPixelBlack(image,p),q);
            float_pixel=(float) (GetPixelAlpha(image,p));
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
        pixel=ScaleQuantumToLong(GetPixelBlack(image,p));
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
            q=PopDoublePixel(quantum_info,(double) GetPixelBlack(image,p),q);
            pixel=(double) (GetPixelAlpha(image,p));
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
        q=PopQuantumPixel(quantum_info,ScaleQuantumToAny(GetPixelRed(image,p),
          range),q);
        q=PopQuantumPixel(quantum_info,ScaleQuantumToAny(GetPixelGreen(image,p),
          range),q);
        q=PopQuantumPixel(quantum_info,ScaleQuantumToAny(GetPixelBlue(image,p),
          range),q);
        q=PopQuantumPixel(quantum_info,ScaleQuantumToAny(GetPixelBlack(image,p),
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