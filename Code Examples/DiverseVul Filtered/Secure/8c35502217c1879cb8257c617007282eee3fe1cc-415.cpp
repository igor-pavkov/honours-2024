void Magick::Image::transformOrigin(const double x_,const double y_)
{
  modifyImage();
  options()->transformOrigin(x_,y_);
}