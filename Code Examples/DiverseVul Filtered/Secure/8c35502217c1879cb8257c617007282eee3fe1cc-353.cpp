const MagickCore::QuantizeInfo *Magick::Image::constQuantizeInfo(void) const
{
  return(_imgRef->options()->quantizeInfo());
}