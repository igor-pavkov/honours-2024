void Magick::Image::border(const Geometry &geometry_)
{
  MagickCore::Image
    *newImage;

  RectangleInfo
    borderInfo=geometry_;

  GetPPException;
  newImage=BorderImage(constImage(),&borderInfo,image()->compose,
    exceptionInfo);
  replaceImage(newImage);
  ThrowImageException;
}