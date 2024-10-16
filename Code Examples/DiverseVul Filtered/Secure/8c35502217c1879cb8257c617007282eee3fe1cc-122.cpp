void Magick::Image::segment(const double clusterThreshold_,
  const double smoothingThreshold_)
{
  modifyImage();
  GetPPException;
  SegmentImage(image(),options()->quantizeColorSpace(),
    (MagickBooleanType) options()->verbose(),clusterThreshold_,
    smoothingThreshold_,exceptionInfo);
  SyncImage(image(),exceptionInfo);
  ThrowImageException;
}