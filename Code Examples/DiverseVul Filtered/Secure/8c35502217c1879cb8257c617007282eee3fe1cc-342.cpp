void Magick::Image::strokeDashArray(const double *strokeDashArray_)
{
  modifyImage();
  options()->strokeDashArray(strokeDashArray_);
}