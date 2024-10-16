void Magick::Image::shave(const Geometry &geometry_)
{
  MagickCore::Image
    *newImage;

  RectangleInfo
    shaveInfo=geometry_;

  GetPPException;
  newImage=ShaveImage(constImage(),&shaveInfo,exceptionInfo);
  replaceImage(newImage);
  ThrowImageException;
}