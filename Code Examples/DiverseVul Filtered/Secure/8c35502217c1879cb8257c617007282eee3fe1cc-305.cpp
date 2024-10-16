void Magick::Image::adjoin(const bool flag_)
{
  modifyImage();
  options()->adjoin(flag_);
}