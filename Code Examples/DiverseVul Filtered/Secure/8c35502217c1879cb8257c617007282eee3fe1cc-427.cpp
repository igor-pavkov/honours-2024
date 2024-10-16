void Magick::Image::gamma(const double gammaRed_,const double gammaGreen_,
  const double gammaBlue_)
{
  modifyImage();
  GetPPException;
  GetAndSetPPChannelMask(RedChannel);
  (void) GammaImage(image(),gammaRed_,exceptionInfo);
  SetPPChannelMask(GreenChannel);
  (void) GammaImage(image(),gammaGreen_,exceptionInfo);
  SetPPChannelMask(BlueChannel);
  (void) GammaImage(image(),gammaBlue_,exceptionInfo);
  RestorePPChannelMask;
  ThrowImageException;
}