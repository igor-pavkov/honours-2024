void Magick::Image::textInterwordSpacing(double spacing_)
{
  modifyImage();
  options()->textInterwordSpacing(spacing_);
}