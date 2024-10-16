void Magick::Image::borderColor(const Color &borderColor_)
{
  modifyImage();

  if (borderColor_.isValid())
    image()->border_color=borderColor_;
  else
    image()->border_color=Color();

  options()->borderColor(borderColor_);
}