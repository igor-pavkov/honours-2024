void Magick::Image::x11Display(const std::string &display_)
{
  modifyImage();
  options()->x11Display(display_);
}