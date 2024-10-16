void Magick::Image::fontPointsize(const double pointSize_)
{
  modifyImage();
  options()->fontPointsize(pointSize_);
}