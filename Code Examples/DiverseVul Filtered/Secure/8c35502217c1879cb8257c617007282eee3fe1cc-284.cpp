void Magick::Image::quantizeDither(const bool ditherFlag_)
{
  modifyImage();
  options()->quantizeDither(ditherFlag_);
}