static Image *ReadWEBPImage(const ImageInfo *image_info,
  ExceptionInfo *exception)
{
#define ThrowWEBPException(severity,tag) \
{ \
  if (stream != (unsigned char *) NULL) \
    stream=(unsigned char*) RelinquishMagickMemory(stream); \
  if (webp_image != (WebPDecBuffer *) NULL) \
    WebPFreeDecBuffer(webp_image); \
  ThrowReaderException(severity,tag); \
}

  Image
    *image;

  int
    webp_status;

  MagickBooleanType
    status;

  register unsigned char
    *p;

  size_t
    length;

  ssize_t
    count,
    y;

  unsigned char
    header[12],
    *stream;

  WebPDecoderConfig
    configure;

  WebPDecBuffer
    *magick_restrict webp_image = &configure.output;

  WebPBitstreamFeatures
    *magick_restrict features = &configure.input;

  /*
    Open image file.
  */
  assert(image_info != (const ImageInfo *) NULL);
  assert(image_info->signature == MagickCoreSignature);
  if (image_info->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",
      image_info->filename);
  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickCoreSignature);
  image=AcquireImage(image_info,exception);
  status=OpenBlob(image_info,image,ReadBinaryBlobMode,exception);
  if (status == MagickFalse)
    {
      image=DestroyImageList(image);
      return((Image *) NULL);
    }
  stream=(unsigned char *) NULL;
  if (WebPInitDecoderConfig(&configure) == 0)
    ThrowReaderException(ResourceLimitError,"UnableToDecodeImageFile");
  webp_image->colorspace=MODE_RGBA;
  count=ReadBlob(image,12,header);
  if (count != 12)
    ThrowWEBPException(CorruptImageError,"InsufficientImageDataInFile");
  status=IsWEBP(header,count);
  if (status == MagickFalse)
    ThrowWEBPException(CorruptImageError,"CorruptImage");
  length=(size_t) (ReadWebPLSBWord(header+4)+8);
  if (length < 12)
    ThrowWEBPException(CorruptImageError,"CorruptImage");
  if (length > GetBlobSize(image))
    ThrowWEBPException(CorruptImageError,"InsufficientImageDataInFile");
  stream=(unsigned char *) AcquireQuantumMemory(length,sizeof(*stream));
  if (stream == (unsigned char *) NULL)
    ThrowWEBPException(ResourceLimitError,"MemoryAllocationFailed");
  (void) memcpy(stream,header,12);
  count=ReadBlob(image,length-12,stream+12);
  if (count != (ssize_t) (length-12))
    ThrowWEBPException(CorruptImageError,"InsufficientImageDataInFile");
  webp_status=WebPGetFeatures(stream,length,features);
  if (webp_status == VP8_STATUS_OK)
    {
      image->columns=(size_t) features->width;
      image->rows=(size_t) features->height;
      image->depth=8;
      image->alpha_trait=features->has_alpha != 0 ? BlendPixelTrait :
        UndefinedPixelTrait;
      if (IsWEBPImageLossless(stream,length) != MagickFalse)
        image->quality=100;
      if (image_info->ping != MagickFalse)
        {
          stream=(unsigned char*) RelinquishMagickMemory(stream);
          (void) CloseBlob(image);
          return(GetFirstImageInList(image));
        }
      status=SetImageExtent(image,image->columns,image->rows,exception);
      if (status == MagickFalse)
        {
          stream=(unsigned char*) RelinquishMagickMemory(stream);
          (void) CloseBlob(image);
          return(DestroyImageList(image));
        }
      webp_status=WebPDecode(stream,length,&configure);
    }
  if (webp_status != VP8_STATUS_OK)
    switch (webp_status)
    {
      case VP8_STATUS_OUT_OF_MEMORY:
      {
        ThrowWEBPException(ResourceLimitError,"MemoryAllocationFailed");
        break;
      }
      case VP8_STATUS_INVALID_PARAM:
      {
        ThrowWEBPException(CorruptImageError,"invalid parameter");
        break;
      }
      case VP8_STATUS_BITSTREAM_ERROR:
      {
        ThrowWEBPException(CorruptImageError,"CorruptImage");
        break;
      }
      case VP8_STATUS_UNSUPPORTED_FEATURE:
      {
        ThrowWEBPException(CoderError,"DataEncodingSchemeIsNotSupported");
        break;
      }
      case VP8_STATUS_SUSPENDED:
      {
        ThrowWEBPException(CorruptImageError,"decoder suspended");
        break;
      }
      case VP8_STATUS_USER_ABORT:
      {
        ThrowWEBPException(CorruptImageError,"user abort");
        break;
      }
      case VP8_STATUS_NOT_ENOUGH_DATA:
      {
        ThrowWEBPException(CorruptImageError,"InsufficientImageDataInFile");
        break;
      }
      default:
        ThrowWEBPException(CorruptImageError,"CorruptImage");
    }
  p=(unsigned char *) webp_image->u.RGBA.rgba;
  for (y=0; y < (ssize_t) image->rows; y++)
  {
    register Quantum
      *q;

    register ssize_t
      x;

    q=QueueAuthenticPixels(image,0,y,image->columns,1,exception);
    if (q == (Quantum *) NULL)
      break;
    for (x=0; x < (ssize_t) image->columns; x++)
    {
      SetPixelRed(image,ScaleCharToQuantum(*p++),q);
      SetPixelGreen(image,ScaleCharToQuantum(*p++),q);
      SetPixelBlue(image,ScaleCharToQuantum(*p++),q);
      SetPixelAlpha(image,ScaleCharToQuantum(*p++),q);
      q+=GetPixelChannels(image);
    }
    if (SyncAuthenticPixels(image,exception) == MagickFalse)
      break;
    status=SetImageProgress(image,LoadImageTag,(MagickOffsetType) y,
      image->rows);
    if (status == MagickFalse)
      break;
  }
  WebPFreeDecBuffer(webp_image);
#if defined(MAGICKCORE_WEBPMUX_DELEGATE)
  {
    StringInfo
      *profile;

    uint32_t
      webp_flags = 0;

    WebPData
     content = { stream, length },
     chunk ={ 0 };

    WebPMux
      *mux;

    /*
      Extract any profiles.
    */
    mux=WebPMuxCreate(&content,0);
    WebPMuxGetFeatures(mux,&webp_flags);
    if (webp_flags & ICCP_FLAG)
      {
        WebPMuxGetChunk(mux,"ICCP",&chunk);
        profile=BlobToStringInfo(chunk.bytes,chunk.size);
        if (profile != (StringInfo *) NULL)
          {
            SetImageProfile(image,"ICC",profile,exception);
            profile=DestroyStringInfo(profile);
          }
      }
    if (webp_flags & EXIF_FLAG)
      {
        WebPMuxGetChunk(mux,"EXIF",&chunk);
        profile=BlobToStringInfo(chunk.bytes,chunk.size);
        if (profile != (StringInfo *) NULL)
          {
            SetImageProfile(image,"EXIF",profile,exception);
            profile=DestroyStringInfo(profile);
          }
      }
    if (webp_flags & XMP_FLAG)
      {
        WebPMuxGetChunk(mux,"XMP",&chunk);
        profile=BlobToStringInfo(chunk.bytes,chunk.size);
        if (profile != (StringInfo *) NULL)
          {
            SetImageProfile(image,"XMP",profile,exception);
            profile=DestroyStringInfo(profile);
          }
      }
    WebPMuxDelete(mux);
  }
#endif
  stream=(unsigned char*) RelinquishMagickMemory(stream);
  (void) CloseBlob(image);
  return(image);
}