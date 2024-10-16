void Magick::Image::textAntiAlias(const bool flag_)
{
  modifyImage();
  options()->textAntiAlias(flag_);
}