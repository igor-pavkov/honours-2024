void Magick::Image::orientation(const Magick::OrientationType orientation_)
{
  modifyImage();
  image()->orientation=orientation_;
}