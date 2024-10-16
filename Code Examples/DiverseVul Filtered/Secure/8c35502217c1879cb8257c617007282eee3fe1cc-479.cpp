void Magick::Image::rotate(const double degrees_)
{
  MagickCore::Image
    *newImage;

  GetPPException;
  newImage=RotateImage(constImage(),degrees_,exceptionInfo);
  replaceImage(newImage);
  ThrowImageException;
}