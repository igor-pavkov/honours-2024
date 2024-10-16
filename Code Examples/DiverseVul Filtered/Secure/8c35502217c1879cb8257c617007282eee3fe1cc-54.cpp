void Magick::Image::orderedDither(std::string thresholdMap_)
{
  modifyImage();
  GetPPException;
  (void) OrderedDitherImage(image(),thresholdMap_.c_str(),exceptionInfo);
  ThrowImageException;
}