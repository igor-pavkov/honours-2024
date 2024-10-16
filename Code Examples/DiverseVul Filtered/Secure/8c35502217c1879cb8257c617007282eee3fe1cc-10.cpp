void Magick::Image::negate(const bool grayscale_)
{
  modifyImage();
  GetPPException;
  NegateImage(image(),(MagickBooleanType) grayscale_,exceptionInfo);
  ThrowImageException;
}