void Magick::Image::quantize(const bool measureError_)
{
  modifyImage();
 
  if (measureError_)
    options()->quantizeInfo()->measure_error=MagickTrue;
  else
    options()->quantizeInfo()->measure_error=MagickFalse;

  GetPPException;
  QuantizeImage(options()->quantizeInfo(),image(),exceptionInfo);
  ThrowImageException;
}