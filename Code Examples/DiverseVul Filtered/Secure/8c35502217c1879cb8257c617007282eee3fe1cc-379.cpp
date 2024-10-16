void Magick::Image::roll(const Geometry &roll_)
{
  MagickCore::Image
    *newImage;

  GetPPException;
  newImage=RollImage(constImage(),roll_.xOff(),roll_.yOff(),exceptionInfo);
  replaceImage(newImage);
  ThrowImageException;
}