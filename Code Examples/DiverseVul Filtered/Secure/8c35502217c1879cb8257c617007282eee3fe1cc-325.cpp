void Magick::Image::autoGammaChannel(const ChannelType channel_)
{
  modifyImage();
  GetPPException;
  GetAndSetPPChannelMask(channel_);
  (void) SyncImageSettings(imageInfo(),image(),exceptionInfo);
  (void) AutoGammaImage(image(),exceptionInfo);
  RestorePPChannelMask;
  ThrowImageException;
}