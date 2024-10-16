void Magick::Image::transformScale(const double sx_,const double sy_)
{
  modifyImage();
  options()->transformScale(sx_,sy_);
}