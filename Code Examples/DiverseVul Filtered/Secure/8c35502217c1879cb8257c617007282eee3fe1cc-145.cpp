void Magick::Image::density(const Point &density_)
{
  modifyImage();
  options()->density(density_);
  if (density_.isValid())
    {
      image()->resolution.x=density_.x();
      if (density_.y() != 0.0)
        image()->resolution.y=density_.y();
      else
        image()->resolution.y=density_.x();
    }
  else
    {
      // Reset to default
      image()->resolution.x=0.0;
      image()->resolution.y=0.0;
    }
}