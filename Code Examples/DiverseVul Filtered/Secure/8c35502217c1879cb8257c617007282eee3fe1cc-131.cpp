void Magick::Image::alphaChannel(AlphaChannelOption alphaOption_)
{
  modifyImage();
  GetPPException;
  SetImageAlphaChannel(image(),alphaOption_,exceptionInfo);
  ThrowImageException;
}