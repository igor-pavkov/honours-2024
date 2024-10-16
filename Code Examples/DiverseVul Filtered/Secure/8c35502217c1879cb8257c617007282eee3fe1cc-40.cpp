void Magick::Image::cdl(const std::string &cdl_)
{
  modifyImage();
  GetPPException;
  (void) ColorDecisionListImage(image(),cdl_.c_str(),exceptionInfo);
  ThrowImageException;
}