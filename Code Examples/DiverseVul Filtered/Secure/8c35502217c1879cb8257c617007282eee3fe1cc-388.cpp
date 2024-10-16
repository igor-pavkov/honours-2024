void Magick::Image::colorize(const unsigned int alphaRed_,
  const unsigned int alphaGreen_,const unsigned int alphaBlue_,
  const Color &penColor_)
{
  char
    blend[MagickPathExtent];

  MagickCore::Image
    *newImage;

  PixelInfo
    target;

  if (!penColor_.isValid())
    throwExceptionExplicit(MagickCore::OptionError,
      "Pen color argument is invalid");

  FormatLocaleString(blend,MagickPathExtent,"%u/%u/%u",alphaRed_,alphaGreen_,
    alphaBlue_);

  target=static_cast<PixelInfo>(penColor_);
  GetPPException;
  newImage=ColorizeImage(image(),blend,&target,exceptionInfo);
  replaceImage(newImage);
  ThrowImageException;
}