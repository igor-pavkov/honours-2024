void Magick::Image::transformRotation(const double angle_)
{
  modifyImage();
  options()->transformRotation(angle_);
}