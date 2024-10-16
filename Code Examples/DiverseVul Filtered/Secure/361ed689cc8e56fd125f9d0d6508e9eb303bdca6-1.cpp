static MagickBooleanType WriteWEBPImage(const ImageInfo *image_info,
  Image *image,ExceptionInfo *exception)
{
  const char
    *value;

  int
    webp_status;

  MagickBooleanType
    status;

  MemoryInfo
    *pixel_info;

  register uint32_t
    *magick_restrict q;

  ssize_t
    y;

  WebPAuxStats
    statistics;

  WebPConfig
    configure;

#if defined(MAGICKCORE_WEBPMUX_DELEGATE)
  WebPMemoryWriter
    writer_info;
#endif

  WebPPicture
    picture;

  /*
    Open output image file.
  */
  assert(image_info != (const ImageInfo *) NULL);
  assert(image_info->signature == MagickCoreSignature);
  assert(image != (Image *) NULL);
  assert(image->signature == MagickCoreSignature);
  if (image->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",image->filename);
  if ((image->columns > 16383UL) || (image->rows > 16383UL))
    ThrowWriterException(ImageError,"WidthOrHeightExceedsLimit");
  status=OpenBlob(image_info,image,WriteBinaryBlobMode,exception);
  if (status == MagickFalse)
    return(status);
  if ((WebPPictureInit(&picture) == 0) || (WebPConfigInit(&configure) == 0))
    ThrowWriterException(ResourceLimitError,"UnableToEncodeImageFile");
#if !defined(MAGICKCORE_WEBPMUX_DELEGATE)
  picture.writer=WebPEncodeWriter;
  picture.custom_ptr=(void *) image;
#else
  WebPMemoryWriterInit(&writer_info);
  picture.writer=WebPMemoryWrite;
  picture.custom_ptr=(&writer_info);
#endif
#if WEBP_DECODER_ABI_VERSION >= 0x0100
  picture.progress_hook=WebPEncodeProgress;
  picture.user_data=(void *) image;
#endif
  picture.stats=(&statistics);
  picture.width=(int) image->columns;
  picture.height=(int) image->rows;
  picture.argb_stride=(int) image->columns;
  picture.use_argb=1;
  if (image->quality != UndefinedCompressionQuality)
    configure.quality=(float) image->quality;
  if (image->quality >= 100)
    configure.lossless=1;
  value=GetImageOption(image_info,"webp:lossless");
  if (value != (char *) NULL)
    configure.lossless=(int) ParseCommandOption(MagickBooleanOptions,
      MagickFalse,value);
  value=GetImageOption(image_info,"webp:method");
  if (value != (char *) NULL)
    configure.method=StringToInteger(value);
  value=GetImageOption(image_info,"webp:image-hint");
  if (value != (char *) NULL)
    {
      if (LocaleCompare(value,"default") == 0)
        configure.image_hint=WEBP_HINT_DEFAULT;
      if (LocaleCompare(value,"photo") == 0)
        configure.image_hint=WEBP_HINT_PHOTO;
      if (LocaleCompare(value,"picture") == 0)
        configure.image_hint=WEBP_HINT_PICTURE;
#if WEBP_DECODER_ABI_VERSION >= 0x0200
      if (LocaleCompare(value,"graph") == 0)
        configure.image_hint=WEBP_HINT_GRAPH;
#endif
    }
  value=GetImageOption(image_info,"webp:target-size");
  if (value != (char *) NULL)
    configure.target_size=StringToInteger(value);
  value=GetImageOption(image_info,"webp:target-psnr");
  if (value != (char *) NULL)
    configure.target_PSNR=(float) StringToDouble(value,(char **) NULL);
  value=GetImageOption(image_info,"webp:segments");
  if (value != (char *) NULL)
    configure.segments=StringToInteger(value);
  value=GetImageOption(image_info,"webp:sns-strength");
  if (value != (char *) NULL)
    configure.sns_strength=StringToInteger(value);
  value=GetImageOption(image_info,"webp:filter-strength");
  if (value != (char *) NULL)
    configure.filter_strength=StringToInteger(value);
  value=GetImageOption(image_info,"webp:filter-sharpness");
  if (value != (char *) NULL)
    configure.filter_sharpness=StringToInteger(value);
  value=GetImageOption(image_info,"webp:filter-type");
  if (value != (char *) NULL)
    configure.filter_type=StringToInteger(value);
  value=GetImageOption(image_info,"webp:auto-filter");
  if (value != (char *) NULL)
    configure.autofilter=(int) ParseCommandOption(MagickBooleanOptions,
      MagickFalse,value);
  value=GetImageOption(image_info,"webp:alpha-compression");
  if (value != (char *) NULL)
    configure.alpha_compression=StringToInteger(value);
  value=GetImageOption(image_info,"webp:alpha-filtering");
  if (value != (char *) NULL)
    configure.alpha_filtering=StringToInteger(value);
  value=GetImageOption(image_info,"webp:alpha-quality");
  if (value != (char *) NULL)
    configure.alpha_quality=StringToInteger(value);
  value=GetImageOption(image_info,"webp:pass");
  if (value != (char *) NULL)
    configure.pass=StringToInteger(value);
  value=GetImageOption(image_info,"webp:show-compressed");
  if (value != (char *) NULL)
    configure.show_compressed=StringToInteger(value);
  value=GetImageOption(image_info,"webp:preprocessing");
  if (value != (char *) NULL)
    configure.preprocessing=StringToInteger(value);
  value=GetImageOption(image_info,"webp:partitions");
  if (value != (char *) NULL)
    configure.partitions=StringToInteger(value);
  value=GetImageOption(image_info,"webp:partition-limit");
  if (value != (char *) NULL)
    configure.partition_limit=StringToInteger(value);
#if WEBP_DECODER_ABI_VERSION >= 0x0201
  value=GetImageOption(image_info,"webp:emulate-jpeg-size");
  if (value != (char *) NULL)
    configure.emulate_jpeg_size=(int) ParseCommandOption(MagickBooleanOptions,
      MagickFalse,value);
  value=GetImageOption(image_info,"webp:low-memory");
  if (value != (char *) NULL)
    configure.low_memory=(int) ParseCommandOption(MagickBooleanOptions,
      MagickFalse,value);
  value=GetImageOption(image_info,"webp:thread-level");
  if (value != (char *) NULL)
    configure.thread_level=StringToInteger(value);
#endif
  if (WebPValidateConfig(&configure) == 0)
    ThrowWriterException(ResourceLimitError,"UnableToEncodeImageFile");
  /*
    Allocate memory for pixels.
  */
  (void) TransformImageColorspace(image,sRGBColorspace,exception);
  pixel_info=AcquireVirtualMemory(image->columns,image->rows*
    sizeof(*picture.argb));
  if (pixel_info == (MemoryInfo *) NULL)
    ThrowWriterException(ResourceLimitError,"MemoryAllocationFailed");
  picture.argb=(uint32_t *) GetVirtualMemoryBlob(pixel_info);
  /*
    Convert image to WebP raster pixels.
  */
  q=picture.argb;
  for (y=0; y < (ssize_t) image->rows; y++)
  {
    register const Quantum
      *magick_restrict p;

    register ssize_t
      x;

    p=GetVirtualPixels(image,0,y,image->columns,1,exception);
    if (p == (const Quantum *) NULL)
      break;
    for (x=0; x < (ssize_t) image->columns; x++)
    {
      *q++=(uint32_t) (image->alpha_trait != UndefinedPixelTrait ?
        ScaleQuantumToChar(GetPixelAlpha(image,p)) << 24 : 0xff000000) |
        (ScaleQuantumToChar(GetPixelRed(image,p)) << 16) |
        (ScaleQuantumToChar(GetPixelGreen(image,p)) << 8) |
        (ScaleQuantumToChar(GetPixelBlue(image,p)));
      p+=GetPixelChannels(image);
    }
    status=SetImageProgress(image,SaveImageTag,(MagickOffsetType) y,
      image->rows);
    if (status == MagickFalse)
      break;
  }
  webp_status=WebPEncode(&configure,&picture);
  if (webp_status == 0)
    {
      const char
        *message;

      switch (picture.error_code)
      {
        case VP8_ENC_ERROR_OUT_OF_MEMORY:
        {
          message="out of memory";
          break;
        }
        case VP8_ENC_ERROR_BITSTREAM_OUT_OF_MEMORY:
        {
          message="bitstream out of memory";
          break;
        }
        case VP8_ENC_ERROR_NULL_PARAMETER:
        {
          message="NULL parameter";
          break;
        }
        case VP8_ENC_ERROR_INVALID_CONFIGURATION:
        {
          message="invalid configuration";
          break;
        }
        case VP8_ENC_ERROR_BAD_DIMENSION:
        {
          message="bad dimension";
          break;
        }
        case VP8_ENC_ERROR_PARTITION0_OVERFLOW:
        {
          message="partition 0 overflow (> 512K)";
          break;
        }
        case VP8_ENC_ERROR_PARTITION_OVERFLOW:
        {
          message="partition overflow (> 16M)";
          break;
        }
        case VP8_ENC_ERROR_BAD_WRITE:
        {
          message="bad write";
          break;
        }
        case VP8_ENC_ERROR_FILE_TOO_BIG:
        {
          message="file too big (> 4GB)";
          break;
        }
#if WEBP_DECODER_ABI_VERSION >= 0x0100
        case VP8_ENC_ERROR_USER_ABORT:
        {
          message="user abort";
          break;
        }
#endif
        default:
        {
          message="unknown exception";
          break;
        }
      }
      (void) ThrowMagickException(exception,GetMagickModule(),CorruptImageError,
        (char *) message,"`%s'",image->filename);
    }
#if defined(MAGICKCORE_WEBPMUX_DELEGATE)
  {
    const StringInfo
      *profile;

    WebPData
      chunk,
      image_chunk = { writer_info.mem, writer_info.size };

    WebPMux
      *mux;

    WebPMuxError
      mux_error;

    /*
      Set image profiles (if any).
    */
    mux_error=WEBP_MUX_OK;
    chunk.size=0;
    mux=WebPMuxNew();
    profile=GetImageProfile(image,"ICC");    
    if ((profile != (StringInfo *) NULL) && (mux_error == WEBP_MUX_OK))
      {
        chunk.bytes=GetStringInfoDatum(profile);
        chunk.size=GetStringInfoLength(profile);
        mux_error=WebPMuxSetChunk(mux,"ICCP",&chunk,0);
      }
    profile=GetImageProfile(image,"EXIF");    
    if ((profile != (StringInfo *) NULL) && (mux_error == WEBP_MUX_OK))
      {
        chunk.bytes=GetStringInfoDatum(profile);
        chunk.size=GetStringInfoLength(profile);
        mux_error=WebPMuxSetChunk(mux,"EXIF",&chunk,0);
      }
    profile=GetImageProfile(image,"XMP");    
    if ((profile != (StringInfo *) NULL) && (mux_error == WEBP_MUX_OK))
      {
        chunk.bytes=GetStringInfoDatum(profile);
        chunk.size=GetStringInfoLength(profile);
        mux_error=WebPMuxSetChunk(mux,"XMP",&chunk,0);
      }
    if (mux_error != WEBP_MUX_OK)
      (void) ThrowMagickException(exception,GetMagickModule(),
        ResourceLimitError,"UnableToEncodeImageFile","`%s'",image->filename);
    if (chunk.size != 0)
      {
        WebPData
          picture_profiles = { writer_info.mem, writer_info.size };

        /*
          Replace original container with image profile (if any).
        */
        WebPMuxSetImage(mux,&image_chunk,1);
        mux_error=WebPMuxAssemble(mux,&picture_profiles);
        WebPMemoryWriterClear(&writer_info);
        writer_info.size=picture_profiles.size;
        writer_info.mem=(unsigned char *) picture_profiles.bytes;
      }
    WebPMuxDelete(mux);
  }
  (void) WriteBlob(image,writer_info.size,writer_info.mem);
#endif
  picture.argb=(uint32_t *) NULL;
  WebPPictureFree(&picture);
#if defined(MAGICKCORE_WEBPMUX_DELEGATE)
  WebPMemoryWriterClear(&writer_info);
#endif
  pixel_info=RelinquishVirtualMemory(pixel_info);
  (void) CloseBlob(image);
  return(webp_status == 0 ? MagickFalse : MagickTrue);
}