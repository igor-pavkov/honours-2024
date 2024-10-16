void Magick::Image::stereo(const Image &rightImage_)
{
  MagickCore::Image
    *newImage;

  GetPPException;
  newImage=StereoImage(constImage(),rightImage_.constImage(),exceptionInfo);
  replaceImage(newImage);
  ThrowImageException;
}