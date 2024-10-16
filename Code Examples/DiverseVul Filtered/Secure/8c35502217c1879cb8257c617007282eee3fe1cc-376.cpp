void Magick::Image::ping(const std::string &imageSpec_)
{
  MagickCore::Image
    *newImage;

  GetPPException;
  options()->fileName(imageSpec_);
  newImage=PingImage(imageInfo(),exceptionInfo);
  read(newImage,exceptionInfo);
}