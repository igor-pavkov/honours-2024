void Magick::Image::levelColors(const Color &blackColor_,
  const Color &whiteColor_,const bool invert_)
{
  PixelInfo
    black,
    white;

  modifyImage();

  black=static_cast<PixelInfo>(blackColor_);
  white=static_cast<PixelInfo>(whiteColor_);
  GetPPException;
  (void) LevelImageColors(image(),&black,&white,invert_ == true ?
    MagickTrue : MagickFalse,exceptionInfo);
  ThrowImageException;
}