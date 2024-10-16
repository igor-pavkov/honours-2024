void Magick::Image::lowlightColor(const Color color_)
{
  std::string
    value;

  value=color_;
  artifact("compare:lowlight-color",value);
}