void Magick::Image::modifyImage(void)
{
  if (!_imgRef->isShared())
    return;

  GetPPException;
  replaceImage(CloneImage(image(),0,0,MagickTrue,exceptionInfo));
  ThrowImageException;
}