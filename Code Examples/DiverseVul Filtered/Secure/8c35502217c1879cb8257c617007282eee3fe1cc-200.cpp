std::string Magick::Image::label(void) const
{
  const char
    *value;

  GetPPException;
  value=GetImageProperty(constImage(),"Label",exceptionInfo);
  ThrowImageException;

  if (value)
    return(std::string(value));

  return(std::string());
}