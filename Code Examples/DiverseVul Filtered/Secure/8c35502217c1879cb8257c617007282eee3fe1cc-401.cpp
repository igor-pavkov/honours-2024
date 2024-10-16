Magick::Geometry Magick::Image::size(void) const
{
  return(Magick::Geometry(constImage()->columns,constImage()->rows));
}