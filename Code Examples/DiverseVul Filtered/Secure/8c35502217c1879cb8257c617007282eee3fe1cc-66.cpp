Magick::ImageType Magick::Image::type(void) const
{
  if (constOptions()->type() != UndefinedType)
    return(constOptions()->type());
  return(GetImageType(constImage()));
}