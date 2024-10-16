void Magick::Image::defineSet(const std::string &magick_,
  const std::string &key_,bool flag_)
{
  std::string
    definition;

  modifyImage();
  definition=magick_ + ":" + key_;
  if (flag_)
    (void) SetImageOption(imageInfo(),definition.c_str(),"");
  else
    DeleteImageOption(imageInfo(),definition.c_str());
}