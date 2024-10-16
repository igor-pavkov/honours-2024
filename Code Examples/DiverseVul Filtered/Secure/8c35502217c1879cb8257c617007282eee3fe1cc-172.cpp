Magick::Image Magick::Image::readMask(void) const
{
  return(mask(ReadPixelMask));
}