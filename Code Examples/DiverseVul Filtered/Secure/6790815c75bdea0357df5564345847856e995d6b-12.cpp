MagickExport void DestroyImageOptions(ImageInfo *image_info)
{
  assert(image_info != (ImageInfo *) NULL);
  assert(image_info->signature == MagickCoreSignature);
  if (image_info->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",
      image_info->filename);
  if (image_info->options != (void *) NULL)
    image_info->options=DestroySplayTree((SplayTreeInfo *) image_info->options);
}