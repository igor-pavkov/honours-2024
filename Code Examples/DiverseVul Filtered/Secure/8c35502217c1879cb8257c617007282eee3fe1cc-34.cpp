void Magick::Image::composite(const Image &compositeImage_,
  const GravityType gravity_,const CompositeOperator compose_)
{
  RectangleInfo
    geometry;

  modifyImage();
  SetGeometry(compositeImage_.constImage(),&geometry);
  GravityAdjustGeometry(columns(),rows(),gravity_,&geometry);

  GetPPException;
  CompositeImage(image(),compositeImage_.constImage(),compose_,MagickTrue,
    geometry.x,geometry.y,exceptionInfo);
  ThrowImageException;
}