void Magick::Image::animationDelay(const size_t delay_)
{
  modifyImage();
  image()->delay=delay_;
}