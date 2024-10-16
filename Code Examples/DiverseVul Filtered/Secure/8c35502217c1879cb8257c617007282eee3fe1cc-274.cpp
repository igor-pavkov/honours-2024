void Magick::Image::writeMask(const Magick::Image &mask_)
{
  mask(mask_,WritePixelMask);
}