void Magick::Image::mask(const Magick::Image &mask_,const PixelMask type)
{
  modifyImage();

  GetPPException;
  if (mask_.isValid())
    SetImageMask(image(),type,mask_.constImage(),exceptionInfo);
  else
    SetImageMask(image(),type,(MagickCore::Image *) NULL,
      exceptionInfo);
  ThrowImageException;
}