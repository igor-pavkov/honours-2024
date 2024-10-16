Magick::ImageMoments Magick::Image::moments(void) const
{
  return(ImageMoments(*this));
}