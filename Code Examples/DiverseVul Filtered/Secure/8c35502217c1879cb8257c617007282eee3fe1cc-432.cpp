void Magick::Image::addNoiseChannel(const ChannelType channel_,
  const NoiseType noiseType_)
{
  MagickCore::Image
    *newImage;

  GetPPException;
  GetAndSetPPChannelMask(channel_);
  newImage=AddNoiseImage(constImage(),noiseType_,1.0,exceptionInfo);
  RestorePPChannelMask;
  replaceImage(newImage);
  ThrowImageException;
}