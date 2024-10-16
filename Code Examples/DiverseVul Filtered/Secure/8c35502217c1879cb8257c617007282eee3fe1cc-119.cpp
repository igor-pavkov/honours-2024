const MagickCore::Image *Magick::Image::constImage(void) const
{
  return(_imgRef->image());
}