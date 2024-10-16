void Magick::Image::textDirection(DirectionType direction_)
{
  modifyImage();
  options()->textDirection(direction_);
}