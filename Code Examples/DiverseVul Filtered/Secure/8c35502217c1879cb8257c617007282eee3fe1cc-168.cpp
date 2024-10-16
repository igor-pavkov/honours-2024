void Magick::Image::debug(const bool flag_)
{
  modifyImage();
  options()->debug(flag_);
}