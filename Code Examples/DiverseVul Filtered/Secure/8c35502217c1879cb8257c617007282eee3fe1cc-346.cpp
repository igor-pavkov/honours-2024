void Magick::Image::contrast(const bool sharpen_)
{
  modifyImage();
  GetPPException;
  ContrastImage(image(),(MagickBooleanType) sharpen_,exceptionInfo);
  ThrowImageException;
}