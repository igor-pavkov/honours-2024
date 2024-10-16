void Magick::Image::verbose(const bool verboseFlag_)
{
  modifyImage();
  options()->verbose(verboseFlag_);
}