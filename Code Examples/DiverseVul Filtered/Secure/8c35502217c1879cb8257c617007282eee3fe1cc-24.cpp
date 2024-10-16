void Magick::Image::composite(const Image &compositeImage_,
  const ssize_t xOffset_,const ssize_t yOffset_,
  const CompositeOperator compose_)
{
  // Image supplied as compositeImage is composited with current image and
  // results in updating current image.
  modifyImage();
  GetPPException;
  CompositeImage(image(),compositeImage_.constImage(),compose_,MagickTrue,
    xOffset_,yOffset_,exceptionInfo);
  ThrowImageException;
}