void Magick::Image::quantizeColorSpace(
  const Magick::ColorspaceType colorSpace_)
{
  modifyImage();
  options()->quantizeColorSpace(colorSpace_);
}