void Magick::Image::blackThreshold(const std::string &threshold_)
{
  modifyImage();
  GetPPException;
  BlackThresholdImage(image(),threshold_.c_str(),exceptionInfo);
  ThrowImageException;
}