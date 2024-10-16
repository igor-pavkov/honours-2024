MagickExport Image *BlobToImage(const ImageInfo *image_info,const void *blob,
  const size_t length,ExceptionInfo *exception)
{
  const MagickInfo
    *magick_info;

  Image
    *image;

  ImageInfo
    *blob_info,
    *clone_info;

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
  blob_info=CloneImageInfo(image_info);
  blob_info->blob=(void *) blob;
  blob_info->length=length;
  if (*blob_info->magick == '\0')
    (void) SetImageInfo(blob_info,0,exception);
  magick_info=GetMagickInfo(blob_info->magick,exception);
  if (magick_info == (const MagickInfo *) NULL)
    {
      (void) ThrowMagickException(exception,GetMagickModule(),
        MissingDelegateError,"NoDecodeDelegateForThisImageFormat","`%s'",
        blob_info->magick);
      blob_info=DestroyImageInfo(blob_info);
      return((Image *) NULL);
    }
  if (GetMagickBlobSupport(magick_info) != MagickFalse)
    {
      char
        filename[MagickPathExtent];

      /*
        Native blob support for this image format.
      */
      (void) CopyMagickString(filename,blob_info->filename,MagickPathExtent);
      (void) FormatLocaleString(blob_info->filename,MaxTextExtent,"%s:%s",
        blob_info->magick,filename);
      image=ReadImage(blob_info,exception);
      if (image != (Image *) NULL)
        (void) DetachBlob(image->blob);
      blob_info=DestroyImageInfo(blob_info);
      return(image);
    }
  /*
    Write blob to a temporary file on disk.
  */
  blob_info->blob=(void *) NULL;
  blob_info->length=0;
  *blob_info->filename='\0';
  status=BlobToFile(blob_info->filename,blob,length,exception);
  if (status == MagickFalse)
    {
      (void) RelinquishUniqueFileResource(blob_info->filename);
      blob_info=DestroyImageInfo(blob_info);
      return((Image *) NULL);
    }
  clone_info=CloneImageInfo(blob_info);
  (void) FormatLocaleString(clone_info->filename,MaxTextExtent,"%s:%s",
    blob_info->magick,blob_info->filename);
  image=ReadImage(clone_info,exception);
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
  (void) RelinquishUniqueFileResource(blob_info->filename);
  blob_info=DestroyImageInfo(blob_info);
  return(image);
}