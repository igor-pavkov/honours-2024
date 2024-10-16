void Magick::Image::strokePattern(const Image &strokePattern_)
{
  modifyImage();
  if(strokePattern_.isValid())
    options()->strokePattern(strokePattern_.constImage());
  else
    options()->strokePattern(static_cast<MagickCore::Image*>(NULL));
}