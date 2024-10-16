void Magick::Image::highlightColor(const Color color_)
{
  std::string
    value;

  value=color_;
  artifact("compare:highlight-color",value);
}