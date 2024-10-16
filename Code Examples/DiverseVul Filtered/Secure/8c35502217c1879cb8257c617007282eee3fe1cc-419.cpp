Magick::Image Magick::Image::compareChannel(const ChannelType channel_,
  const Image &reference_,const MetricType metric_,double *distortion)
{
  MagickCore::Image
    *newImage;

  GetPPException;
  GetAndSetPPChannelMask(channel_);
  newImage=CompareImages(image(),reference_.constImage(),metric_,distortion,
    exceptionInfo);
  RestorePPChannelMask;
  ThrowImageException;
  if (newImage == (MagickCore::Image *) NULL)
    return(Magick::Image());
  else
    return(Magick::Image(newImage));
}