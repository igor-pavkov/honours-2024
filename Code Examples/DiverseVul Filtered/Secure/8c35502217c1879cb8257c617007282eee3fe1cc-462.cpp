void Magick::Image::autoThreshold(const AutoThresholdMethod method_)
{
  modifyImage();
  GetPPException;
  AutoThresholdImage(image(),method_, exceptionInfo);
  ThrowImageException;
}