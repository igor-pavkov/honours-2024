static MagickBooleanType WriteTGAImage(const ImageInfo *image_info,Image *image,
  ExceptionInfo *exception)
{
  CompressionType
    compression;

  const char
    *comment,
    *value;

  const double
    midpoint = QuantumRange/2.0;

  MagickBooleanType
    status;

  QuantumAny
    range;

  register const Quantum
    *p;

  register ssize_t
    x;

  register ssize_t
    i;

  register unsigned char
    *q;

  size_t
    channels;

  ssize_t
    count,
    y;

  TGAInfo
    tga_info;

  /*
    Open output image file.
  */
  assert(image_info != (const ImageInfo *) NULL);
  assert(image_info->signature == MagickCoreSignature);
  assert(image != (Image *) NULL);
  assert(image->signature == MagickCoreSignature);
  if (image->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",image->filename);
  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickCoreSignature);
  status=OpenBlob(image_info,image,WriteBinaryBlobMode,exception);
  if (status == MagickFalse)
    return(status);
  /*
    Initialize TGA raster file header.
  */
  if ((image->columns > 65535L) || (image->rows > 65535L))
    ThrowWriterException(ImageError,"WidthOrHeightExceedsLimit");
  (void) TransformImageColorspace(image,sRGBColorspace,exception);
  compression=image->compression;
  if (image_info->compression != UndefinedCompression)
    compression=image_info->compression;
  range=GetQuantumRange(5UL);
  tga_info.id_length=0;
  comment=GetImageProperty(image,"comment",exception);
  if (comment != (const char *) NULL)
    tga_info.id_length=(unsigned char) MagickMin(strlen(comment),255);
  tga_info.colormap_type=0;
  tga_info.colormap_index=0;
  tga_info.colormap_length=0;
  tga_info.colormap_size=0;
  tga_info.x_origin=0;
  tga_info.y_origin=0;
  tga_info.width=(unsigned short) image->columns;
  tga_info.height=(unsigned short) image->rows;
  tga_info.bits_per_pixel=8;
  tga_info.attributes=0;
  if ((image_info->type != TrueColorType) &&
      (image_info->type != TrueColorAlphaType) &&
      (image_info->type != PaletteType) &&
      (image->alpha_trait == UndefinedPixelTrait) &&
      (SetImageGray(image,exception) != MagickFalse))
    tga_info.image_type=compression == RLECompression ? TGARLEMonochrome :
      TGAMonochrome;
  else
    if ((image->storage_class == DirectClass) || (image->colors > 256))
      {
        /*
          Full color TGA raster.
        */
        tga_info.image_type=compression == RLECompression ? TGARLERGB : TGARGB;
        if (image_info->depth == 5)
          {
            tga_info.bits_per_pixel=16;
            if (image->alpha_trait != UndefinedPixelTrait)
              tga_info.attributes=1;  /* # of alpha bits */
          }
        else
          {
            tga_info.bits_per_pixel=24;
            if (image->alpha_trait != UndefinedPixelTrait)
              {
                tga_info.bits_per_pixel=32;
                tga_info.attributes=8;  /* # of alpha bits */
              }
          }
      }
    else
      {
        /*
          Colormapped TGA raster.
        */
        tga_info.image_type=compression == RLECompression ? TGARLEColormap :
          TGAColormap;
        tga_info.colormap_type=1;
        tga_info.colormap_length=(unsigned short) image->colors;
        if (image_info->depth == 5)
          tga_info.colormap_size=16;
        else
          tga_info.colormap_size=24;
      }
  value=GetImageArtifact(image,"tga:image-origin");
  if (value != (const char *) NULL)
    {
      OrientationType
        origin;

      origin=(OrientationType) ParseCommandOption(MagickOrientationOptions,
        MagickFalse,value);
      if (origin == BottomRightOrientation || origin == TopRightOrientation)
        tga_info.attributes|=(1UL << 4);
      if (origin == TopLeftOrientation || origin == TopRightOrientation)
        tga_info.attributes|=(1UL << 5);
    }
  /*
    Write TGA header.
  */
  (void) WriteBlobByte(image,tga_info.id_length);
  (void) WriteBlobByte(image,tga_info.colormap_type);
  (void) WriteBlobByte(image,(unsigned char) tga_info.image_type);
  (void) WriteBlobLSBShort(image,tga_info.colormap_index);
  (void) WriteBlobLSBShort(image,tga_info.colormap_length);
  (void) WriteBlobByte(image,tga_info.colormap_size);
  (void) WriteBlobLSBShort(image,tga_info.x_origin);
  (void) WriteBlobLSBShort(image,tga_info.y_origin);
  (void) WriteBlobLSBShort(image,tga_info.width);
  (void) WriteBlobLSBShort(image,tga_info.height);
  (void) WriteBlobByte(image,tga_info.bits_per_pixel);
  (void) WriteBlobByte(image,tga_info.attributes);
  if (tga_info.id_length != 0)
    (void) WriteBlob(image,tga_info.id_length,(unsigned char *) comment);
  if (tga_info.colormap_type != 0)
    {
      unsigned char
        green,
        *targa_colormap;

      /*
        Dump colormap to file (blue, green, red byte order).
      */
      targa_colormap=(unsigned char *) AcquireQuantumMemory((size_t)
        tga_info.colormap_length,(tga_info.colormap_size/8)*
        sizeof(*targa_colormap));
      if (targa_colormap == (unsigned char *) NULL)
        ThrowWriterException(ResourceLimitError,"MemoryAllocationFailed");
      q=targa_colormap;
      for (i=0; i < (ssize_t) image->colors; i++)
      {
        if (image_info->depth == 5)
          {
            green=(unsigned char) ScaleQuantumToAny(ClampToQuantum(
              image->colormap[i].green),range);
            *q++=((unsigned char) ScaleQuantumToAny(ClampToQuantum(
              image->colormap[i].blue),range)) | ((green & 0x07) << 5);
            *q++=(((image->alpha_trait != UndefinedPixelTrait) && ((double)
              ClampToQuantum(image->colormap[i].alpha) > midpoint)) ? 0x80 : 0) |
              ((unsigned char) ScaleQuantumToAny(ClampToQuantum(
              image->colormap[i].red),range) << 2) | ((green & 0x18) >> 3);
          }
        else
          {
            *q++=ScaleQuantumToChar(ClampToQuantum(image->colormap[i].blue));
            *q++=ScaleQuantumToChar(ClampToQuantum(image->colormap[i].green));
            *q++=ScaleQuantumToChar(ClampToQuantum(image->colormap[i].red));
          }
      }
      (void) WriteBlob(image,(size_t) ((tga_info.colormap_size/8)*
        tga_info.colormap_length),targa_colormap);
      targa_colormap=(unsigned char *) RelinquishMagickMemory(targa_colormap);
    }
  /*
    Convert MIFF to TGA raster pixels.
  */
  channels=GetPixelChannels(image);
  for (y=(ssize_t) (image->rows-1); y >= 0; y--)
  {
    p=GetVirtualPixels(image,0,y,image->columns,1,exception);
    if (p == (const Quantum *) NULL)
      break;
    if (compression == RLECompression)
      {
        x=0;
        count=0;
        while (x < (ssize_t) image->columns)
        {
          i=1;
          while ((i < 128) && (count + i < 128) &&
                 ((x + i) < (ssize_t) image->columns))
          {
            if (tga_info.image_type == TGARLEColormap)
              {
                if (GetPixelIndex(image,p+(i*channels)) !=
                    GetPixelIndex(image,p+((i-1)*channels)))
                  break;
              }
            else if (tga_info.image_type == TGARLEMonochrome)
              {
                if (GetPixelLuma(image,p+(i*channels)) !=
                    GetPixelLuma(image,p+((i-1)*channels)))
                  break;
              }
            else
              {
                if ((GetPixelBlue(image,p+(i*channels)) !=
                     GetPixelBlue(image,p+((i-1)*channels))) ||
                    (GetPixelGreen(image,p+(i*channels)) !=
                     GetPixelGreen(image,p+((i-1)*channels))) ||
                    (GetPixelRed(image,p+(i*channels)) !=
                     GetPixelRed(image,p+((i-1)*channels))))
                  break;
                if ((image->alpha_trait != UndefinedPixelTrait) &&
                    (GetPixelAlpha(image,p+(i*channels)) !=
                     GetPixelAlpha(image,p+(i-1)*channels)))
                  break;
              }
            i++;
          }
          if (i < 3)
            {
              count+=i;
              p+=(i*channels);
            }
          if ((i >= 3) || (count == 128) ||
              ((x + i) == (ssize_t) image->columns))
            {
              if (count > 0)
                {
                  (void) WriteBlobByte(image,(unsigned char) (--count));
                  while (count >= 0)
                  {
                    WriteTGAPixel(image,tga_info.image_type,p-((count+1)*
                      channels),range,midpoint);
                    count--;
                  }
                  count=0;
                }
            }
          if (i >= 3)
            {
              (void) WriteBlobByte(image,(unsigned char) ((i-1) | 0x80));
              WriteTGAPixel(image,tga_info.image_type,p,range,midpoint);
              p+=(i*channels);
            }
          x+=i;
        }
      }
    else
      {
        for (x=0; x < (ssize_t) image->columns; x++)
          {
            WriteTGAPixel(image,tga_info.image_type,p,range,midpoint);
            p+=channels;
          }
      }
    if (image->previous == (Image *) NULL)
      {
        status=SetImageProgress(image,SaveImageTag,(MagickOffsetType) y,
          image->rows);
        if (status == MagickFalse)
          break;
      }
  }
  (void) CloseBlob(image);
  return(MagickTrue);
}