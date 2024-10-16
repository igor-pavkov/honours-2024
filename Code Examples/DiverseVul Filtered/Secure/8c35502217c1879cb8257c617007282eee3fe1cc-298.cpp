void Magick::Image::strokeAntiAlias(const bool flag_)
{
  modifyImage();
  options()->strokeAntiAlias(flag_);
}