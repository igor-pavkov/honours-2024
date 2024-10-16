void Magick::Image::floodFillColor(const ssize_t x_,const ssize_t y_,
  const Magick::Color &fillColor_,const Magick::Color &borderColor_,
  const bool invert_)
{
  PixelInfo
    pixel;

  modifyImage();

  pixel=static_cast<PixelInfo>(borderColor_);
  floodFill(x_,y_,(Magick::Image *)NULL,fillColor_,&pixel,invert_);
}