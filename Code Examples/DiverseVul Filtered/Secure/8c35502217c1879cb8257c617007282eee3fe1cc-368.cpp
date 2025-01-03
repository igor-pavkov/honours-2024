void Magick::Image::floodFillAlpha(const ssize_t x_,const ssize_t y_,
  const unsigned int alpha_,const bool invert_)
{
  PixelInfo
    target;

  modifyImage();

  target=static_cast<PixelInfo>(pixelColor(x_,y_));
  target.alpha=alpha_;
  GetPPException;
  GetAndSetPPChannelMask(AlphaChannel);
  FloodfillPaintImage(image(),options()->drawInfo(),&target,x_,y_,
    (MagickBooleanType)invert_,exceptionInfo);
  RestorePPChannelMask;
  ThrowImageException;
}