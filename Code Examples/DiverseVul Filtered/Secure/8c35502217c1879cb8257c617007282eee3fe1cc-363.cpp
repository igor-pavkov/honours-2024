void Magick::Image::map(const Image &mapImage_,const bool dither_)
{
  modifyImage();
  GetPPException;
  options()->quantizeDither(dither_);
  RemapImage(options()->quantizeInfo(),image(),mapImage_.constImage(),
    exceptionInfo);
  ThrowImageException;
}