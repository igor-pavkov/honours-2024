void Magick::Image::display(void)
{
  GetPPException;
  DisplayImages(imageInfo(),image(),exceptionInfo);
  ThrowImageException;
}