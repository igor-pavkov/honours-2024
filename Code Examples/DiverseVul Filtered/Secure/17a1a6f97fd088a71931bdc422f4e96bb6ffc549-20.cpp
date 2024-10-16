static inline void SetPSDPixel(Image *image,const size_t channels,
  const ssize_t type,const size_t packet_size,const Quantum pixel,Quantum *q,
  ExceptionInfo *exception)
{
  if (image->storage_class == PseudoClass)
    {
      PixelInfo
        *color;

      if (type == 0)
        {
          if (packet_size == 1)
            SetPixelIndex(image,ScaleQuantumToChar(pixel),q);
          else
            SetPixelIndex(image,ScaleQuantumToShort(pixel),q);
        }
      color=image->colormap+(ssize_t) ConstrainColormapIndex(image,
        GetPixelIndex(image,q),exception);
      if ((type == 0) && (channels > 1))
        return;
      else
        color->alpha=(MagickRealType) pixel;
      SetPixelViaPixelInfo(image,color,q);
      return;
    }
  switch (type)
  {
    case -1:
    {
      SetPixelAlpha(image,pixel,q);
      break;
    }
    case -2:
    case 0:
    {
      SetPixelRed(image,pixel,q);
      break;
    }
    case -3:
    case 1:
    {
      SetPixelGreen(image,pixel,q);
      break;
    }
    case -4:
    case 2:
    {
      SetPixelBlue(image,pixel,q);
      break;
    }
    case 3:
    {
      if (image->colorspace == CMYKColorspace)
        SetPixelBlack(image,pixel,q);
      else
        if (image->alpha_trait != UndefinedPixelTrait)
          SetPixelAlpha(image,pixel,q);
      break;
    }
    case 4:
    {
      if ((IssRGBCompatibleColorspace(image->colorspace) != MagickFalse) &&
          (channels > 3))
        break;
      if (image->alpha_trait != UndefinedPixelTrait)
        SetPixelAlpha(image,pixel,q);
      break;
    }
  }
}