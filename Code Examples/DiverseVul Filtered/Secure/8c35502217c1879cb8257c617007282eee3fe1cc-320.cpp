void Magick::Image::strokeDashOffset(const double strokeDashOffset_)
{
  modifyImage();
  options()->strokeDashOffset(strokeDashOffset_);
}