MagickExport void SetImageInfoCustomStream(ImageInfo *image_info,
  CustomStreamInfo *custom_stream)
{
  assert(image_info != (ImageInfo *) NULL);
  assert(image_info->signature == MagickCoreSignature);
  if (image_info->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",
      image_info->filename);
  image_info->custom_stream=(CustomStreamInfo *) custom_stream;
}