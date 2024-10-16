void Magick::Image::virtualPixelMethod(
  const VirtualPixelMethod virtualPixelMethod_)
{
  modifyImage();
  GetPPException;
  SetImageVirtualPixelMethod(image(),virtualPixelMethod_,exceptionInfo);
  ThrowImageException;
}