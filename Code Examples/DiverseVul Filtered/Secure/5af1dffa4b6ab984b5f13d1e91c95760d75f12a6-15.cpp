MagickExport size_t ExportQuantumPixels(const Image *image,
  CacheView *image_view,QuantumInfo *quantum_info,
  const QuantumType quantum_type,unsigned char *magick_restrict pixels,
  ExceptionInfo *exception)
{
  MagickSizeType
    number_pixels;

  const Quantum
    *magick_restrict p;

  ssize_t
    x;

  unsigned char
    *magick_restrict q;

  size_t
    extent;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickCoreSignature);
  if (image->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",image->filename);
  assert(quantum_info != (QuantumInfo *) NULL);
  assert(quantum_info->signature == MagickCoreSignature);
  if (pixels == (unsigned char *) NULL)
    pixels=(unsigned char *) GetQuantumPixels(quantum_info);
  if (image_view == (CacheView *) NULL)
    {
      number_pixels=GetImageExtent(image);
      p=GetVirtualPixelQueue(image);
    }
  else
    {
      number_pixels=GetCacheViewExtent(image_view);
      p=GetCacheViewVirtualPixelQueue(image_view);
    }
  if (quantum_info->alpha_type == AssociatedQuantumAlpha)
    {
      double
        Sa;

      Quantum
        *magick_restrict q;

      /*
        Associate alpha.
      */
      if (image_view != (CacheView *) NULL)
        q=GetCacheViewAuthenticPixelQueue(image_view);
      else
        q=GetAuthenticPixelQueue(image);
      for (x=0; x < (ssize_t) image->columns; x++)
      {
        ssize_t
          i;

        Sa=QuantumScale*GetPixelAlpha(image,q);
        for (i=0; i < (ssize_t) GetPixelChannels(image); i++)
        {
          PixelChannel channel = GetPixelChannelChannel(image,i);
          PixelTrait traits = GetPixelChannelTraits(image,channel);
          if ((traits & UpdatePixelTrait) == 0)
            continue;
          q[i]=ClampToQuantum(Sa*q[i]);
        }
        q+=GetPixelChannels(image);
      }
    }
  if ((quantum_type == CbYCrQuantum) || (quantum_type == CbYCrAQuantum))
    {
      Quantum
        quantum;

      Quantum
        *magick_restrict q;

      if (image_view != (CacheView *) NULL)
        q=GetAuthenticPixelQueue(image);
      else
        q=GetAuthenticPixelQueue(image);
      for (x=0; x < (ssize_t) number_pixels; x++)
      {
        quantum=GetPixelRed(image,q);
        SetPixelRed(image,GetPixelGreen(image,q),q);
        SetPixelGreen(image,quantum,q);
        q+=GetPixelChannels(image);
      }
    }
  q=pixels;
  ResetQuantumState(quantum_info);
  extent=GetQuantumExtent(image,quantum_info,quantum_type);
  switch (quantum_type)
  {
    case AlphaQuantum:
    {
      ExportAlphaQuantum(image,quantum_info,number_pixels,p,q,exception);
      break;
    }
    case BGRQuantum:
    {
      ExportBGRQuantum(image,quantum_info,number_pixels,p,q,exception);
      break;
    }
    case BGRAQuantum:
    {
      ExportBGRAQuantum(image,quantum_info,number_pixels,p,q,exception);
      break;
    }
    case BGROQuantum:
    {
      ExportBGROQuantum(image,quantum_info,number_pixels,p,q,exception);
      break;
    }
    case BlackQuantum:
    {
      ExportBlackQuantum(image,quantum_info,number_pixels,p,q,exception);
      break;
    }
    case BlueQuantum:
    case YellowQuantum:
    {
      ExportBlueQuantum(image,quantum_info,number_pixels,p,q,exception);
      break;
    }
    case CMYKQuantum:
    {
      ExportCMYKQuantum(image,quantum_info,number_pixels,p,q,exception);
      break;
    }
    case CMYKAQuantum:
    {
      ExportCMYKAQuantum(image,quantum_info,number_pixels,p,q,exception);
      break;
    }
    case CMYKOQuantum:
    {
      ExportCMYKOQuantum(image,quantum_info,number_pixels,p,q,exception);
      break;
    }
    case CbYCrYQuantum:
    {
      ExportCbYCrYQuantum(image,quantum_info,number_pixels,p,q,exception);
      break;
    }
    case GrayQuantum:
    {
      ExportGrayQuantum(image,quantum_info,number_pixels,p,q,exception);
      break;
    }
    case GrayAlphaQuantum:
    {
      ExportGrayAlphaQuantum(image,quantum_info,number_pixels,p,q,exception);
      break;
    }
    case GreenQuantum:
    case MagentaQuantum:
    {
      ExportGreenQuantum(image,quantum_info,number_pixels,p,q,exception);
      break;
    }
    case IndexQuantum:
    {
      ExportIndexQuantum(image,quantum_info,number_pixels,p,q,exception);
      break;
    }
    case IndexAlphaQuantum:
    {
      ExportIndexAlphaQuantum(image,quantum_info,number_pixels,p,q,exception);
      break;
    }
    case RedQuantum:
    case CyanQuantum:
    {
      ExportRedQuantum(image,quantum_info,number_pixels,p,q,exception);
      break;
    }
    case OpacityQuantum:
    {
      ExportOpacityQuantum(image,quantum_info,number_pixels,p,q,exception);
      break;
    }
    case RGBQuantum:
    case CbYCrQuantum:
    {
      ExportRGBQuantum(image,quantum_info,number_pixels,p,q,exception);
      break;
    }
    case RGBAQuantum:
    case CbYCrAQuantum:
    {
      ExportRGBAQuantum(image,quantum_info,number_pixels,p,q,exception);
      break;
    }
    case RGBOQuantum:
    {
      ExportRGBOQuantum(image,quantum_info,number_pixels,p,q,exception);
      break;
    }
    default:
      break;
  }
  if ((quantum_type == CbYCrQuantum) || (quantum_type == CbYCrAQuantum))
    {
      Quantum
        quantum;

      Quantum
        *magick_restrict q;

      if (image_view != (CacheView *) NULL)
        q=GetCacheViewAuthenticPixelQueue(image_view);
      else
        q=GetAuthenticPixelQueue(image);
      for (x=0; x < (ssize_t) number_pixels; x++)
      {
        quantum=GetPixelRed(image,q);
        SetPixelRed(image,GetPixelGreen(image,q),q);
        SetPixelGreen(image,quantum,q);
        q+=GetPixelChannels(image);
      }
    }
  return(extent);
}