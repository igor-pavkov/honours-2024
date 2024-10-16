void Magick::Image::strokeColor(const Magick::Color &strokeColor_)
{
  std::string
    value;

  modifyImage();
  options()->strokeColor(strokeColor_);
  value=strokeColor_;
  artifact("stroke",value);
}