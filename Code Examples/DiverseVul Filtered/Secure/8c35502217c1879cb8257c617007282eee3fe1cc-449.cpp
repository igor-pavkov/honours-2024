void Magick::Image::type(const Magick::ImageType type_)
{
  modifyImage();
  options()->type(type_);
  GetPPException;
  SetImageType(image(),type_,exceptionInfo);
  ThrowImageException;
}