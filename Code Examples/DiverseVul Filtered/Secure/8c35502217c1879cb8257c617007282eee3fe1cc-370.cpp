void Magick::Image::quantizeColors(const size_t colors_)
{
  modifyImage();
  options()->quantizeColors(colors_);
}