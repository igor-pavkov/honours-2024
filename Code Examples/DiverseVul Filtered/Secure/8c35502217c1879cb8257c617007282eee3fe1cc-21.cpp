void Magick::Image::cannyEdge(const double radius_,const double sigma_,
  const double lowerPercent_,const double upperPercent_)
{
  MagickCore::Image
    *newImage;

  modifyImage();
  GetPPException;
  newImage=CannyEdgeImage(constImage(),radius_,sigma_,lowerPercent_,
    upperPercent_,exceptionInfo);
  replaceImage(newImage);
  ThrowImageException;
}