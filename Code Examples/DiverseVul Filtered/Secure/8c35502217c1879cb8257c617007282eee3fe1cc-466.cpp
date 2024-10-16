void Magick::Image::floodFillTexture(const ssize_t x_,const ssize_t y_,
  const Magick::Image &texture_,const Magick::Color &borderColor_,
  const bool invert_)
{
  PixelInfo
    pixel;

  modifyImage();

  pixel=static_cast<PixelInfo>(borderColor_);
  floodFill(x_,y_,&texture_,Magick::Color(),&pixel,invert_);
}