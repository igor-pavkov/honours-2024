void Magick::Image::threshold(const double threshold_)
{
  modifyImage();
  GetPPException;
  BilevelImage(image(),threshold_,exceptionInfo);
  ThrowImageException;
}