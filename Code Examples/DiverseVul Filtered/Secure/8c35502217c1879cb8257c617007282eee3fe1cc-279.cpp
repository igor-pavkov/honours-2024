void Magick::Image::colorSpaceType(const ColorspaceType colorSpace_)
{
  modifyImage();
  GetPPException;
  SetImageColorspace(image(),colorSpace_,exceptionInfo);
  ThrowImageException;
  options()->colorspaceType(colorSpace_);
}