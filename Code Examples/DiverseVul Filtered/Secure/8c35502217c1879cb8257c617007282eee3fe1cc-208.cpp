void Magick::Image::defineValue(const std::string &magick_,
  const std::string &key_,const std::string &value_)
{
  std::string
    format,
    option;

  modifyImage();
  format=magick_ + ":" + key_;
  option=value_;
  (void) SetImageOption(imageInfo(),format.c_str(),option.c_str());
}