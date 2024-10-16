const MagickCore::ImageInfo *Magick::Image::constImageInfo(void) const
{
  return(_imgRef->options()->imageInfo());
}