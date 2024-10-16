void Magick::Image::subRange(const size_t subRange_)
{
  modifyImage();
  options()->subRange(subRange_);
}