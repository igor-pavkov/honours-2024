void Magick::Image::colorSpace(const ColorspaceType colorSpace_)
{
  if (image()->colorspace == colorSpace_)
    return;

  modifyImage();
  GetPPException;
  TransformImageColorspace(image(),colorSpace_,exceptionInfo);
  ThrowImageException;
}