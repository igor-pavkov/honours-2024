void Magick::Image::attribute(const std::string name_,const char *value_)
{
  modifyImage();
  GetPPException;
  SetImageProperty(image(),name_.c_str(),value_,exceptionInfo);
  ThrowImageException;
}