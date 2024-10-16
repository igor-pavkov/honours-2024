bool Magick::Image::isOpaque(void) const
{
  MagickBooleanType
    result;

  GetPPException;
  result=IsImageOpaque(constImage(),exceptionInfo);
  ThrowImageException;
  return(result != MagickFalse ? true : false);
}