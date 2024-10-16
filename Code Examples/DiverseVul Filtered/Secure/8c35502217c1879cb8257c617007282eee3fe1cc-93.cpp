void Magick::Image::repage()
{
  modifyImage();
  options()->page(Geometry());
  image()->page.width = 0;
  image()->page.height = 0;
  image()->page.x = 0;
  image()->page.y = 0;
}