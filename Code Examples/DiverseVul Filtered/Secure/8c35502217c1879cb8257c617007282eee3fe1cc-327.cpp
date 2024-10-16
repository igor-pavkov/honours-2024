void Magick::Image::raise(const Geometry &geometry_,const bool raisedFlag_)
{
  RectangleInfo
    raiseInfo=geometry_;

  GetPPException;
  modifyImage();
  RaiseImage(image(),&raiseInfo,raisedFlag_ == true ? MagickTrue : MagickFalse,
    exceptionInfo);
  ThrowImageException;
}