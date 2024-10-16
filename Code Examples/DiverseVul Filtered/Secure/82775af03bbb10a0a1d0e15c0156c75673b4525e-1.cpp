static MagickBooleanType ReadDCMPixels(Image *image,DCMInfo *info,
  DCMStreamInfo *stream_info,MagickBooleanType first_segment,
  ExceptionInfo *exception)
{
  int
    byte,
    index;

  MagickBooleanType
    status;

  PixelPacket
    pixel;

  ssize_t
    i,
    x;

  Quantum
    *q;

  ssize_t
    y;

  /*
    Convert DCM Medical image to pixel packets.
  */
  byte=0;
  i=0;
  status=MagickTrue;
  (void) memset(&pixel,0,sizeof(pixel));
  for (y=0; y < (ssize_t) image->rows; y++)
  {
    q=QueueAuthenticPixels(image,0,y,image->columns,1,exception);
    if (q == (Quantum *) NULL)
      break;
    for (x=0; x < (ssize_t) image->columns; x++)
    {
      if (info->samples_per_pixel == 1)
        {
          int
            pixel_value;

          if (info->bytes_per_pixel == 1)
            pixel_value=info->polarity != MagickFalse ?
              ((int) info->max_value-ReadDCMByte(stream_info,image)) :
              ReadDCMByte(stream_info,image);
          else
            if ((info->bits_allocated != 12) || (info->significant_bits != 12))
              {
                if (info->signed_data != 0)
                  pixel_value=ReadDCMSignedShort(stream_info,image);
                else
                  pixel_value=(int) ReadDCMShort(stream_info,image);
                if (info->polarity != MagickFalse)
                  pixel_value=(int)info->max_value-pixel_value;
              }
            else
              {
                if ((i & 0x01) != 0)
                  {
                    pixel_value=byte;
                    byte=ReadDCMByte(stream_info,image);
                    if (byte >= 0)
                      pixel_value|=(byte << 8);
                  }
                else
                  {
                    pixel_value=ReadDCMSignedShort(stream_info,image);
                    byte=(int) (pixel_value & 0x0f);
                    pixel_value>>=4;
                  }
                i++;
              }
          if (info->signed_data == 1)
            pixel_value-=32767;
          index=pixel_value;
          if (info->rescale != MagickFalse)
            {
              double
                scaled_value;

              scaled_value=pixel_value*info->rescale_slope+
                info->rescale_intercept;
              index=(int) scaled_value;
              if (info->window_width != 0)
                {
                  double
                    window_max,
                    window_min;

                  window_min=ceil(info->window_center-
                    (info->window_width-1.0)/2.0-0.5);
                  window_max=floor(info->window_center+
                    (info->window_width-1.0)/2.0+0.5);
                  if (scaled_value <= window_min)
                    index=0;
                  else
                    if (scaled_value > window_max)
                      index=(int) info->max_value;
                    else
                      index=(int) (info->max_value*(((scaled_value-
                        info->window_center-0.5)/(info->window_width-1))+0.5));
                }
            }
          index&=info->mask;
          index=(int) ConstrainColormapIndex(image,(ssize_t) index,exception);
          if (first_segment != MagickFalse)
            SetPixelIndex(image,(Quantum) index,q);
          else
            SetPixelIndex(image,(Quantum) (((size_t) index) |
              (((size_t) GetPixelIndex(image,q)) << 8)),q);
          pixel.red=(unsigned int) image->colormap[index].red;
          pixel.green=(unsigned int) image->colormap[index].green;
          pixel.blue=(unsigned int) image->colormap[index].blue;
        }
      else
        {
          if (info->bytes_per_pixel == 1)
            {
              pixel.red=(unsigned int) ReadDCMByte(stream_info,image);
              pixel.green=(unsigned int) ReadDCMByte(stream_info,image);
              pixel.blue=(unsigned int) ReadDCMByte(stream_info,image);
            }
          else
            {
              pixel.red=ReadDCMShort(stream_info,image);
              pixel.green=ReadDCMShort(stream_info,image);
              pixel.blue=ReadDCMShort(stream_info,image);
            }
          pixel.red&=info->mask;
          pixel.green&=info->mask;
          pixel.blue&=info->mask;
          if (info->scale != (Quantum *) NULL)
            {
              if ((MagickSizeType) pixel.red <= GetQuantumRange(info->depth))
                pixel.red=(unsigned int) info->scale[pixel.red];
              if ((MagickSizeType) pixel.green <= GetQuantumRange(info->depth))
                pixel.green=(unsigned int) info->scale[pixel.green];
              if ((MagickSizeType) pixel.blue <= GetQuantumRange(info->depth))
                pixel.blue=(unsigned int) info->scale[pixel.blue];
            }
        }
      if (first_segment != MagickFalse)
        {
          SetPixelRed(image,(Quantum) pixel.red,q);
          SetPixelGreen(image,(Quantum) pixel.green,q);
          SetPixelBlue(image,(Quantum) pixel.blue,q);
        }
      else
        {
          SetPixelRed(image,(Quantum) (((size_t) pixel.red) |
            (((size_t) GetPixelRed(image,q)) << 8)),q);
          SetPixelGreen(image,(Quantum) (((size_t) pixel.green) |
            (((size_t) GetPixelGreen(image,q)) << 8)),q);
          SetPixelBlue(image,(Quantum) (((size_t) pixel.blue) |
            (((size_t) GetPixelBlue(image,q)) << 8)),q);
        }
      q+=GetPixelChannels(image);
    }
    if (SyncAuthenticPixels(image,exception) == MagickFalse)
      break;
    if (image->previous == (Image *) NULL)
      {
        status=SetImageProgress(image,LoadImageTag,(MagickOffsetType) y,
          image->rows);
        if (status == MagickFalse)
          break;
      }
  }
  return(status);
}