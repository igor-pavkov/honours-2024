double Magick::Image::compareChannel(const ChannelType channel_,
  const Image &reference_,const MetricType metric_)
{
  double
    distortion=0.0;

  GetPPException;
  GetAndSetPPChannelMask(channel_);
  GetImageDistortion(image(),reference_.constImage(),metric_,&distortion,
    exceptionInfo);
  RestorePPChannelMask;
  ThrowImageException;
  return(distortion);
}