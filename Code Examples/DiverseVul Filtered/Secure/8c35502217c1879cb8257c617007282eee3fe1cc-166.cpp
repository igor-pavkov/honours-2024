void Magick::Image::fillPattern(const Image &fillPattern_)
{
  modifyImage();
  if (fillPattern_.isValid())
    options()->fillPattern(fillPattern_.constImage());
  else
    options()->fillPattern(static_cast<MagickCore::Image*>(NULL));
}