void Magick::Image::interpolate(const PixelInterpolateMethod interpolate_)
{
  modifyImage();
  image()->interpolate=interpolate_;
}