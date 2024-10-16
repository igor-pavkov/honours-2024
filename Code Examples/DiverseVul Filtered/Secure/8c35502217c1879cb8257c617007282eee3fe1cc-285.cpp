Magick::ImageType Magick::Image::identifyType(void) const
{
  ImageType
    image_type;

  GetPPException;
  image_type=IdentifyImageType(constImage(),exceptionInfo);
  ThrowImageException;
  return(image_type);
}