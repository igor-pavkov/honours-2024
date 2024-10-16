bool Magick::Image::defineSet(const std::string &magick_,
  const std::string &key_ ) const
{
  const char
    *option;

  std::string
    key;

  key=magick_ + ":" + key_;
  option=GetImageOption(constImageInfo(),key.c_str());
  if (option)
    return(true);
  return(false);
}