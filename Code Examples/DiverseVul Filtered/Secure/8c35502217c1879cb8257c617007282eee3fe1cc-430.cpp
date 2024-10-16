MagickCore::MagickSizeType Magick::Image::fileSize(void) const
{
  return(GetBlobSize(constImage()));
}