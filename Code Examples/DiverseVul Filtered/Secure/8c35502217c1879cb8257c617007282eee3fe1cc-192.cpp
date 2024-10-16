std::string Magick::Image::comment(void) const
{
  const char
    *value;

  GetPPException;
  value=GetImageProperty(constImage(),"Comment",exceptionInfo);
  ThrowImageException;

  if (value)
    return(std::string(value));

  return(std::string()); // Intentionally no exception
}