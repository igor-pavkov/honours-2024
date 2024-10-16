MagickExport const char *GetImageOption(const ImageInfo *image_info,
  const char *option)
{
  assert(image_info != (ImageInfo *) NULL);
  assert(image_info->signature == MagickCoreSignature);
  if (image_info->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",
      image_info->filename);
  if (image_info->options == (void *) NULL)
    return((const char *) NULL);
  return((const char *) GetValueFromSplayTree((SplayTreeInfo *)
    image_info->options,option));
}