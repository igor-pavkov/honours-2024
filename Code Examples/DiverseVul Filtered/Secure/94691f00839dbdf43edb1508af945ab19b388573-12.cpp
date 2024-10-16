MagickExport MagickBooleanType MinMaxStretchImage(Image *image,
  const double black,const double white,const double gamma,
  ExceptionInfo *exception)
{
  double
    min,
    max;

  register ssize_t
    i;

  MagickStatusType
    status;

  status=MagickTrue;
  if (image->channel_mask == DefaultChannels)
    {
      /*
        Auto-level all channels equally.
      */
      (void) GetImageRange(image,&min,&max,exception);
      min+=black;
      max-=white;
      if (fabs(min-max) >= MagickEpsilon)
        status&=LevelImage(image,min,max,gamma,exception);
      return(status != 0 ? MagickTrue : MagickFalse);
    }
  /*
    Auto-level each channel.
  */
  for (i=0; i < (ssize_t) GetPixelChannels(image); i++)
  {
    ChannelType
      channel_mask;

    PixelChannel channel = GetPixelChannelChannel(image,i);
    PixelTrait traits = GetPixelChannelTraits(image,channel);
    if ((traits & UpdatePixelTrait) == 0)
      continue;
    channel_mask=SetImageChannelMask(image,(ChannelType) (1UL << i));
    status&=GetImageRange(image,&min,&max,exception);
    min+=black;
    max-=white;
    if (fabs(min-max) >= MagickEpsilon)
      status&=LevelImage(image,min,max,gamma,exception);
    (void) SetImageChannelMask(image,channel_mask);
  }
  return(status != 0 ? MagickTrue : MagickFalse);
}