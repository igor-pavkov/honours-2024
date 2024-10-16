MagickExport MagickBooleanType ClipImagePath(Image *image,const char *pathname,
  const MagickBooleanType inside,ExceptionInfo *exception)
{
#define ClipImagePathTag  "ClipPath/Image"

  char
    *property;

  const char
    *value;

  Image
    *clip_mask;

  ImageInfo
    *image_info;

  assert(image != (const Image *) NULL);
  assert(image->signature == MagickCoreSignature);
  if (image->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",image->filename);
  assert(pathname != NULL);
  property=AcquireString(pathname);
  (void) FormatLocaleString(property,MagickPathExtent,"8BIM:1999,2998:%s",
    pathname);
  value=GetImageProperty(image,property,exception);
  property=DestroyString(property);
  if (value == (const char *) NULL)
    {
      ThrowFileException(exception,OptionError,"NoClipPathDefined",
        image->filename);
      return(MagickFalse);
    }
  image_info=AcquireImageInfo();
  (void) CopyMagickString(image_info->filename,image->filename,
     MagickPathExtent);
  (void) ConcatenateMagickString(image_info->filename,pathname,
    MagickPathExtent);
  clip_mask=BlobToImage(image_info,value,strlen(value),exception);
  image_info=DestroyImageInfo(image_info);
  if (clip_mask == (Image *) NULL)
    return(MagickFalse);
  if (clip_mask->storage_class == PseudoClass)
    {
      (void) SyncImage(clip_mask,exception);
      if (SetImageStorageClass(clip_mask,DirectClass,exception) == MagickFalse)
        return(MagickFalse);
    }
  if (inside == MagickFalse)
    (void) NegateImage(clip_mask,MagickFalse,exception);
  (void) FormatLocaleString(clip_mask->magick_filename,MagickPathExtent,
    "8BIM:1999,2998:%s\nPS",pathname);
  (void) SetImageMask(image,WritePixelMask,clip_mask,exception);
  clip_mask=DestroyImage(clip_mask);
  return(MagickTrue);
}