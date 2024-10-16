std::string Magick::Image::defineValue(const std::string &magick_,
  const std::string &key_) const
{
  const char
    *option;

  std::string
    definition;

  definition=magick_ + ":" + key_;
  option=GetImageOption(constImageInfo(),definition.c_str());
  if (option)
    return(std::string(option));
  return(std::string());
}