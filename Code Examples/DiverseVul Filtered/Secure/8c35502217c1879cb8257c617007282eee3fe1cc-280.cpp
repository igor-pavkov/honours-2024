void Magick::Image::quiet(const bool quiet_)
{
  modifyImage();
  options()->quiet(quiet_);
}