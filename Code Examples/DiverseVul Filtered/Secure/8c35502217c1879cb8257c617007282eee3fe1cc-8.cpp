void Magick::Image::splice(const Geometry &geometry_)
{
  MagickCore::Image
    *newImage;

  RectangleInfo
    spliceInfo=geometry_;

  GetPPException;
  newImage=SpliceImage(constImage(),&spliceInfo,exceptionInfo);
  replaceImage(newImage);
  ThrowImageException;
}