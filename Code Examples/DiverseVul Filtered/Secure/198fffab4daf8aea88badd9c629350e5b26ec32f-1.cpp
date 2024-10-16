static MagickBooleanType ReadPSDChannel(Image *image,const PSDInfo *psd_info,
  LayerInfo* layer_info,const size_t channel,
  const PSDCompressionType compression,ExceptionInfo *exception)
{
  Image
    *channel_image,
    *mask;

  MagickOffsetType
    offset;

  MagickBooleanType
    status;

  channel_image=image;
  mask=(Image *) NULL;
  if (layer_info->channel_info[channel].type < -1)
  {
    /*
      Ignore mask that is not a user supplied layer mask, if the mask is
      disabled or if the flags have unsupported values.
    */
    if (layer_info->channel_info[channel].type != -2 ||
        (layer_info->mask.flags > 3) || (layer_info->mask.flags & 0x02))
    {
      SeekBlob(image,layer_info->channel_info[channel].size-2,SEEK_CUR);
      return(MagickTrue);
    }
    mask=CloneImage(image,layer_info->mask.page.width,
      layer_info->mask.page.height,MagickFalse,exception);
    SetImageType(mask,GrayscaleType,exception);
    channel_image=mask;
  }

  offset=TellBlob(channel_image);
  status=MagickTrue;
  switch(compression)
  {
    case Raw:
      status=ReadPSDChannelRaw(channel_image,psd_info->channels,
        layer_info->channel_info[channel].type,exception);
      break;
    case RLE:
      {
        MagickOffsetType
          *offsets;

        offsets=ReadPSDRLEOffsets(channel_image,psd_info,channel_image->rows);
        if (offsets == (MagickOffsetType *) NULL)
          ThrowBinaryException(ResourceLimitError,"MemoryAllocationFailed",
            image->filename);
        status=ReadPSDChannelRLE(channel_image,psd_info,
          layer_info->channel_info[channel].type,offsets,exception);
        offsets=(MagickOffsetType *) RelinquishMagickMemory(offsets);
      }
      break;
    case ZipWithPrediction:
    case ZipWithoutPrediction:
#ifdef MAGICKCORE_ZLIB_DELEGATE
      status=ReadPSDChannelZip(channel_image,layer_info->channels,
        layer_info->channel_info[channel].type,compression,
        layer_info->channel_info[channel].size-2,exception);
#else
      SeekBlob(image,offset+layer_info->channel_info[channel].size-2,SEEK_SET);
      (void) ThrowMagickException(exception,GetMagickModule(),
          MissingDelegateWarning,"DelegateLibrarySupportNotBuiltIn",
            "'%s' (ZLIB)",image->filename);
#endif
      break;
    default:
      SeekBlob(image,offset+layer_info->channel_info[channel].size-2,SEEK_SET);
      (void) ThrowMagickException(exception,GetMagickModule(),TypeWarning,
        "CompressionNotSupported","'%.20g'",(double) compression);
      break;
  }

  if (status == MagickFalse)
    {
      if (mask != (Image *) NULL)
        DestroyImage(mask);
      SeekBlob(image,offset+layer_info->channel_info[channel].size-2,SEEK_SET);
      ThrowBinaryException(CoderError,"UnableToDecompressImage",
        image->filename);
    }
  if (mask != (Image *) NULL)
  {
    if (status != MagickFalse)
      {
        PixelInfo
          color;

        layer_info->mask.image=CloneImage(image,image->columns,image->rows,
          MagickTrue,exception);
        layer_info->mask.image->alpha_trait=UndefinedPixelTrait;
        GetPixelInfo(layer_info->mask.image,&color);
        color.red=layer_info->mask.background == 0 ? 0 : QuantumRange;
        SetImageColor(layer_info->mask.image,&color,exception);
        (void) CompositeImage(layer_info->mask.image,mask,OverCompositeOp,
          MagickTrue,layer_info->mask.page.x,layer_info->mask.page.y,
          exception);
      }
    DestroyImage(mask);
  }

  return(status);
}