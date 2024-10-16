Magick::Geometry Magick::Image::page(void) const
{
  return(Geometry(constImage()->page.width,constImage()->page.height,
    constImage()->page.x,constImage()->page.y));
}