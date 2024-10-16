void Magick::Image::fillColor(const Magick::Color &fillColor_)
{
  modifyImage();
  options()->fillColor(fillColor_);
}