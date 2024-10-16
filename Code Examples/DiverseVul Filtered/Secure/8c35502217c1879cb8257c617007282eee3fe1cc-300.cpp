void Magick::Image::attribute(const std::string name_,const std::string value_)
{
  modifyImage();
  GetPPException;
  SetImageProperty(image(),name_.c_str(),value_.c_str(),exceptionInfo);
  ThrowImageException;
}