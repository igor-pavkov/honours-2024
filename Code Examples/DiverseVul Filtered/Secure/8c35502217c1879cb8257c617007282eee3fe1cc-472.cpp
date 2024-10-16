void Magick::Image::textInterlineSpacing(double spacing_)
{
  modifyImage();
  options()->textInterlineSpacing(spacing_);
}