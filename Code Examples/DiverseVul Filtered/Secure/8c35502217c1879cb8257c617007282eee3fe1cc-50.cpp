void Magick::Image::write(const std::string &imageSpec_)
{
  modifyImage();
  fileName(imageSpec_);
  GetPPException;
  WriteImage(constImageInfo(),image(),exceptionInfo);
  ThrowImageException;
}