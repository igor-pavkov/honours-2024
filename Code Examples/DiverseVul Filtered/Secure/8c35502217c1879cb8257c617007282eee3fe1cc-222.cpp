void Magick::Image::font(const std::string &font_)
{
  modifyImage();
  options()->font(font_);
}