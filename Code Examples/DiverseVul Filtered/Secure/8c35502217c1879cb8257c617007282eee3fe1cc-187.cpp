void Magick::Image::whiteThreshold(const std::string &threshold_)
{
  modifyImage();
  GetPPException;
  WhiteThresholdImage(image(),threshold_.c_str(),exceptionInfo);
  ThrowImageException;
}