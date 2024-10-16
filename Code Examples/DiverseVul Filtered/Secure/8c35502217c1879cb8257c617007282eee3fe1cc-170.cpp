void Magick::Image::shear(const double xShearAngle_,const double yShearAngle_)
{
  MagickCore::Image
    *newImage;

  GetPPException;
  newImage=ShearImage(constImage(),xShearAngle_,yShearAngle_,exceptionInfo);
  replaceImage(newImage);
  ThrowImageException;
}