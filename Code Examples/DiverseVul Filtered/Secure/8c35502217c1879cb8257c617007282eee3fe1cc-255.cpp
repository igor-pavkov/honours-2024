void Magick::Image::quantizeDitherMethod(const DitherMethod ditherMethod_)
{
  modifyImage();
  options()->quantizeDitherMethod(ditherMethod_);
}