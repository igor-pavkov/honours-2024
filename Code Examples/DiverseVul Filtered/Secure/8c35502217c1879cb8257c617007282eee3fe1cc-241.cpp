void Magick::Image::randomThreshold(const double low_,const double high_)
{
  GetPPException;
  (void) RandomThresholdImage(image(),low_,high_,exceptionInfo);
  ThrowImageException;
}