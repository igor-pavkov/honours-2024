std::string Magick::Image::attribute(const std::string name_) const
{
  const char
    *value;

  GetPPException;
  value=GetImageProperty(constImage(),name_.c_str(),exceptionInfo);
  ThrowImageException;

  if (value)
    return(std::string(value));

  return(std::string()); // Intentionally no exception
}