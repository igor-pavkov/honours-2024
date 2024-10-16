void Magick::Image::backgroundColor(const Color &backgroundColor_)
{
  modifyImage();

  if (backgroundColor_.isValid())
    image()->background_color=backgroundColor_;
  else
    image()->background_color=Color();

  options()->backgroundColor(backgroundColor_);
}