Magick::ImageStatistics Magick::Image::statistics() const
{
  return(ImageStatistics(*this));
}