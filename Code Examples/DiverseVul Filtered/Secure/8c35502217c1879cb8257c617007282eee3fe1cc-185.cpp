size_t Magick::Image::totalColors(void) const
{
  size_t
    colors;

  GetPPException;
  colors=GetNumberColors(constImage(),(FILE *) NULL,exceptionInfo);
  ThrowImageException;
  return colors;
}