void Magick::Image::cycleColormap(const ssize_t amount_)
{
  modifyImage();
  GetPPException;
  CycleColormapImage(image(),amount_,exceptionInfo);
  ThrowImageException;
}