void Magick::Image::extent(const Geometry &geometry_ )
{
  MagickCore::Image
    *newImage;

  RectangleInfo
    extentInfo=geometry_;

  modifyImage();
  extentInfo.x=geometry_.xOff();
  extentInfo.y=geometry_.yOff();
  GetPPException;
  newImage=ExtentImage(image(),&extentInfo,exceptionInfo);
  replaceImage(newImage);
  ThrowImageException;
}