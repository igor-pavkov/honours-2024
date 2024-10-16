void Magick::Image::crop(const Geometry &geometry_)
{
  MagickCore::Image
    *newImage;

  RectangleInfo
    cropInfo=geometry_;

  GetPPException;
  newImage=CropImage(constImage(),&cropInfo,exceptionInfo);
  replaceImage(newImage);
  ThrowImageException;
}