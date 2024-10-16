void Magick::Image::textUnderColor(const Color &underColor_)
{
  modifyImage();
  options()->textUnderColor(underColor_);
}