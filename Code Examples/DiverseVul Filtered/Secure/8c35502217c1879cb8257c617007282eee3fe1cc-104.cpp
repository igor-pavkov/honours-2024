void Magick::Image::masklightColor(const Color color_)
{
  std::string
    value;

  value=color_;
  artifact("compare:masklight-color",value);
}