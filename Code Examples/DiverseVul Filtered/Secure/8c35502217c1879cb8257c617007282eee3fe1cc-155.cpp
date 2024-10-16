void Magick::Image::floodFill(const ssize_t x_,const ssize_t y_,
  const Magick::Image *fillPattern_,const Magick::Color &fill_,
  const MagickCore::PixelInfo *target_,const bool invert_)
{
  Magick::Color
    fillColor;

  MagickCore::Image
    *fillPattern;

  // Set drawing fill pattern or fill color
  fillColor=options()->fillColor();
  fillPattern=(MagickCore::Image *)NULL;
  if (options()->fillPattern() != (MagickCore::Image *)NULL)
    {
      GetPPException;
      fillPattern=CloneImage(options()->fillPattern(),0,0,MagickTrue,
        exceptionInfo);
      ThrowImageException;
    }

  if (fillPattern_ == (Magick::Image *)NULL)
    {
      options()->fillPattern((MagickCore::Image *)NULL);
      options()->fillColor(fill_);
    }
  else
    options()->fillPattern(fillPattern_->constImage());

  GetPPException;
  (void) FloodfillPaintImage(image(),options()->drawInfo(),
    target_,static_cast<ssize_t>(x_),static_cast<ssize_t>(y_),
    (MagickBooleanType) invert_,exceptionInfo);

  options()->fillColor(fillColor);
  options()->fillPattern(fillPattern);
  ThrowImageException;
}