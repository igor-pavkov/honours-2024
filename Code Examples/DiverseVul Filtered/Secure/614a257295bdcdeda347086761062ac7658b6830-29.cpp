MagickExport Image *PingBlob(const ImageInfo *image_info,const void *blob,
  const size_t length,ExceptionInfo *exception)
{
  const MagickInfo
    *magick_info;

  Image
    *image;

  ImageInfo
    *clone_info,
    *ping_info;

  MagickBooleanType
    status;

  assert(image_info != (ImageInfo *) NULL);
  assert(image_info->signature == MagickCoreSignature);
  if (image_info->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",
      image_info->filename);
  assert(exception != (ExceptionInfo *) NULL);
  if ((blob == (const void *) NULL) || (length == 0))
    {
      (void) ThrowMagickException(exception,GetMagickModule(),BlobError,
        "ZeroLengthBlobNotPermitted","`%s'",image_info->filename);
      return((Image *) NULL);
    }
  ping_info=CloneImageInfo(image_info);
  ping_info->blob=(void *) blob;
  ping_info->length=length;
  ping_info->ping=MagickTrue;
  if (*ping_info->magick == '\0')
    (void) SetImageInfo(ping_info,0,exception);
  magick_info=GetMagickInfo(ping_info->magick,exception);
  if (magick_info == (const MagickInfo *) NULL)
    {
      (void) ThrowMagickException(exception,GetMagickModule(),
        MissingDelegateError,"NoDecodeDelegateForThisImageFormat","`%s'",
        ping_info->magick);
      ping_info=DestroyImageInfo(ping_info);
      return((Image *) NULL);
    }
  if (GetMagickBlobSupport(magick_info) != MagickFalse)
    {
      char
        filename[MagickPathExtent];

      /*
        Native blob support for this image format.
      */
      (void) CopyMagickString(filename,ping_info->filename,MagickPathExtent);
      (void) FormatLocaleString(ping_info->filename,MaxTextExtent,"%s:%s",
        ping_info->magick,filename);
      image=ReadStream(ping_info,&PingStream,exception);
      if (image != (Image *) NULL)
        (void) DetachBlob(image->blob);
      ping_info=DestroyImageInfo(ping_info);
      return(image);
    }
  /*
    Write blob to a temporary file on disk.
  */
  ping_info->blob=(void *) NULL;
  ping_info->length=0;
  *ping_info->filename='\0';
  status=BlobToFile(ping_info->filename,blob,length,exception);
  if (status == MagickFalse)
    {
      (void) RelinquishUniqueFileResource(ping_info->filename);
      ping_info=DestroyImageInfo(ping_info);
      return((Image *) NULL);
    }
  clone_info=CloneImageInfo(ping_info);
  (void) FormatLocaleString(clone_info->filename,MaxTextExtent,"%s:%s",
    ping_info->magick,ping_info->filename);
  image=ReadStream(clone_info,&PingStream,exception);
  if (image != (Image *) NULL)
    {
      Image
        *images;

      /*
        Restore original filenames and image format.
      */
      for (images=GetFirstImageInList(image); images != (Image *) NULL; )
      {
        (void) CopyMagickString(images->filename,image_info->filename,
          MaxTextExtent);
        (void) CopyMagickString(images->magick_filename,image_info->filename,
          MaxTextExtent);
        (void) CopyMagickString(images->magick,magick_info->name,MaxTextExtent);
        images=GetNextImageInList(images);
      }
    }
  clone_info=DestroyImageInfo(clone_info);
  (void) RelinquishUniqueFileResource(ping_info->filename);
  ping_info=DestroyImageInfo(ping_info);
  return(image);
}