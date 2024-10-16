MagickExport Image *SeparateImages(const Image *image,ExceptionInfo *exception)
{
  Image
    *images,
    *separate_image;

  register ssize_t
    i;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickCoreSignature);
  if (image->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",image->filename);
  images=NewImageList();
  for (i=0; i < (ssize_t) GetPixelChannels(image); i++)
  {
    PixelChannel channel = GetPixelChannelChannel(image,i);
    PixelTrait traits = GetPixelChannelTraits(image,channel);
    if ((traits == UndefinedPixelTrait) || ((traits & UpdatePixelTrait) == 0))
      continue;
    separate_image=SeparateImage(image,(ChannelType) (1UL << channel),
      exception);
    if (separate_image != (Image *) NULL)
      AppendImageToList(&images,separate_image);
  }
  if (images == (Image *) NULL)
    images=SeparateImage(image,UndefinedChannel,exception);
  return(images);
}