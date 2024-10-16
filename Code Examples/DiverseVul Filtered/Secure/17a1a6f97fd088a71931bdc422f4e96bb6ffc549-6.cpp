static MagickBooleanType WritePSDLayersInternal(Image *image,
  const ImageInfo *image_info,const PSDInfo *psd_info,size_t *layers_size,
  ExceptionInfo *exception)
{
  char
    layer_name[MagickPathExtent];

  const char
    *property;

  const StringInfo
    *info;

  Image
    *base_image,
    *next_image;

  MagickBooleanType
    status;

  MagickOffsetType
    *layer_size_offsets,
    size_offset;

  register ssize_t
    i;

  size_t
    layer_count,
    layer_index,
    length,
    name_length,
    rounded_size,
    size;

  status=MagickTrue;
  base_image=GetNextImageInList(image);
  if (base_image == (Image *) NULL)
    base_image=image;
  size=0;
  size_offset=TellBlob(image);
  SetPSDSize(psd_info,image,0);
  layer_count=0;
  for (next_image=base_image; next_image != NULL; )
  {
    layer_count++;
    next_image=GetNextImageInList(next_image);
  }
  if (image->alpha_trait != UndefinedPixelTrait)
    size+=WriteBlobShort(image,-(unsigned short) layer_count);
  else
    size+=WriteBlobShort(image,(unsigned short) layer_count);
  layer_size_offsets=(MagickOffsetType *) AcquireQuantumMemory(
    (size_t) layer_count,sizeof(MagickOffsetType));
  if (layer_size_offsets == (MagickOffsetType *) NULL)
    ThrowWriterException(ResourceLimitError,"MemoryAllocationFailed");
  layer_index=0;
  for (next_image=base_image; next_image != NULL; )
  {
    Image
      *mask;

    unsigned char
      default_color;

    unsigned short
      channels,
      total_channels;

    mask=(Image *) NULL;
    property=GetImageArtifact(next_image,"psd:opacity-mask");
    default_color=0;
    if (property != (const char *) NULL)
      {
        mask=(Image *) GetImageRegistry(ImageRegistryType,property,exception);
        default_color=strlen(property) == 9 ? 255 : 0;
      }
    size+=WriteBlobSignedLong(image,(signed int) next_image->page.y);
    size+=WriteBlobSignedLong(image,(signed int) next_image->page.x);
    size+=WriteBlobSignedLong(image,(signed int) (next_image->page.y+
      next_image->rows));
    size+=WriteBlobSignedLong(image,(signed int) (next_image->page.x+
      next_image->columns));
    channels=1U;
    if ((next_image->storage_class != PseudoClass) &&
        (IsImageGray(next_image) == MagickFalse))
      channels=next_image->colorspace == CMYKColorspace ? 4U : 3U;
    total_channels=channels;
    if (next_image->alpha_trait != UndefinedPixelTrait)
      total_channels++;
    if (mask != (Image *) NULL)
      total_channels++;
    size+=WriteBlobShort(image,total_channels);
    layer_size_offsets[layer_index++]=TellBlob(image);
    for (i=0; i < (ssize_t) channels; i++)
      size+=WriteChannelSize(psd_info,image,(signed short) i);
    if (next_image->alpha_trait != UndefinedPixelTrait)
      size+=WriteChannelSize(psd_info,image,-1);
    if (mask != (Image *) NULL)
      size+=WriteChannelSize(psd_info,image,-2);
    size+=WriteBlobString(image,image->endian == LSBEndian ? "MIB8" :"8BIM");
    size+=WriteBlobString(image,CompositeOperatorToPSDBlendMode(next_image));
    property=GetImageArtifact(next_image,"psd:layer.opacity");
    if (property != (const char *) NULL)
      {
        Quantum
          opacity;

        opacity=(Quantum) StringToInteger(property);
        size+=WriteBlobByte(image,ScaleQuantumToChar(opacity));
        (void) ApplyPSDLayerOpacity(next_image,opacity,MagickTrue,exception);
      }
    else
      size+=WriteBlobByte(image,255);
    size+=WriteBlobByte(image,0);
    size+=WriteBlobByte(image,next_image->compose==NoCompositeOp ?
      1 << 0x02 : 1); /* layer properties - visible, etc. */
    size+=WriteBlobByte(image,0);
    info=GetAdditionalInformation(image_info,next_image,exception);
    property=(const char *) GetImageProperty(next_image,"label",exception);
    if (property == (const char *) NULL)
      {
        (void) FormatLocaleString(layer_name,MagickPathExtent,"L%.20g",
          (double) layer_index);
        property=layer_name;
      }
    name_length=strlen(property)+1;
    if ((name_length % 4) != 0)
      name_length+=(4-(name_length % 4));
    if (info != (const StringInfo *) NULL)
      name_length+=GetStringInfoLength(info);
    name_length+=8;
    if (mask != (Image *) NULL)
      name_length+=20;
    size+=WriteBlobLong(image,(unsigned int) name_length);
    if (mask == (Image *) NULL)
      size+=WriteBlobLong(image,0);
    else
      {
        if (mask->compose != NoCompositeOp)
          (void) ApplyPSDOpacityMask(next_image,mask,ScaleCharToQuantum(
            default_color),MagickTrue,exception);
        mask->page.y+=image->page.y;
        mask->page.x+=image->page.x;
        size+=WriteBlobLong(image,20);
        size+=WriteBlobSignedLong(image,mask->page.y);
        size+=WriteBlobSignedLong(image,mask->page.x);
        size+=WriteBlobSignedLong(image,(const signed int) mask->rows+
          mask->page.y);
        size+=WriteBlobSignedLong(image,(const signed int) mask->columns+
          mask->page.x);
        size+=WriteBlobByte(image,default_color);
        size+=WriteBlobByte(image,mask->compose == NoCompositeOp ? 2 : 0);
        size+=WriteBlobMSBShort(image,0);
      }
    size+=WriteBlobLong(image,0);
    size+=WritePascalString(image,property,4);
    if (info != (const StringInfo *) NULL)
      size+=WriteBlob(image,GetStringInfoLength(info),
        GetStringInfoDatum(info));
    next_image=GetNextImageInList(next_image);
  }
  /*
    Now the image data!
  */
  next_image=base_image;
  layer_index=0;
  while (next_image != NULL)
  {
    length=WritePSDChannels(psd_info,image_info,image,next_image,
      layer_size_offsets[layer_index++],MagickTrue,exception);
    if (length == 0)
      {
        status=MagickFalse;
        break;
      }
    size+=length;
    next_image=GetNextImageInList(next_image);
  }
  /*
    Write the total size
  */
  if (layers_size != (size_t*) NULL)
    *layers_size=size;
  if ((size/2) != ((size+1)/2))
    rounded_size=size+1;
  else
    rounded_size=size;
  (void) WritePSDSize(psd_info,image,rounded_size,size_offset);
  layer_size_offsets=(MagickOffsetType *) RelinquishMagickMemory(
    layer_size_offsets);
  /*
    Remove the opacity mask from the registry
  */
  next_image=base_image;
  while (next_image != (Image *) NULL)
  {
    property=GetImageArtifact(next_image,"psd:opacity-mask");
    if (property != (const char *) NULL)
      DeleteImageRegistry(property);
    next_image=GetNextImageInList(next_image);
  }

  return(status);
}