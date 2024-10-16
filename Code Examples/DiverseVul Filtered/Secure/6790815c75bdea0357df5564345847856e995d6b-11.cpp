MagickExport MagickBooleanType CloneImageOptions(ImageInfo *image_info,
  const ImageInfo *clone_info)
{
  assert(image_info != (ImageInfo *) NULL);
  assert(image_info->signature == MagickCoreSignature);
  if (image_info->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",
      image_info->filename);
  assert(clone_info != (const ImageInfo *) NULL);
  assert(clone_info->signature == MagickCoreSignature);
  if (clone_info->options != (void *) NULL)
    {
      if (image_info->options != (void *) NULL)
        DestroyImageOptions(image_info);
      image_info->options=CloneSplayTree((SplayTreeInfo *) clone_info->options,
        (void *(*)(void *)) ConstantString,(void *(*)(void *)) ConstantString);
    }
  return(MagickTrue);
}