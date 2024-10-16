void Magick::Image::monochrome(const bool monochromeFlag_)
{
  modifyImage();
  options()->monochrome(monochromeFlag_);
}