void Magick::Image::read(const std::string &imageSpec_)
{
  MagickCore::Image
    *newImage;

  GetPPException;
  options()->fileName(imageSpec_);
  newImage=ReadImage(imageInfo(),exceptionInfo);
  read(newImage,exceptionInfo);
}