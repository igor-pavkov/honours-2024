Magick::Point Magick::Image::density(void) const
{
  if (isValid())
    {
      ssize_t
        x_resolution=72,
        y_resolution=72;

      if (constImage()->resolution.x > 0.0)
        x_resolution=constImage()->resolution.x;

      if (constImage()->resolution.y > 0.0)
        y_resolution=constImage()->resolution.y;

      return(Point(x_resolution,y_resolution));
    }

  return(constOptions()->density());
}