void Magick::Image::animationIterations(const size_t iterations_)
{
  modifyImage();
  image()->iterations=iterations_;
}