void Magick::Image::chop(const Geometry &geometry_)
{
  MagickCore::Image
    *newImage;

  RectangleInfo
    chopInfo=geometry_;

  GetPPException;
  newImage=ChopImage(image(),&chopInfo,exceptionInfo);
  replaceImage(newImage);
  ThrowImageException;
}