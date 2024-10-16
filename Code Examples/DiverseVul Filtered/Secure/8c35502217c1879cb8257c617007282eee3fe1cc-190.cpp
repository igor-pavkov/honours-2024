Magick::VirtualPixelMethod Magick::Image::virtualPixelMethod(void) const
{
  return(GetImageVirtualPixelMethod(constImage()));
}