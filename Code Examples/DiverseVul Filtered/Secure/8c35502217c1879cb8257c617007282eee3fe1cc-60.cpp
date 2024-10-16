void Magick::Image::transparentChroma(const Color &colorLow_,
  const Color &colorHigh_)
{
  std::string
    colorHigh,
    colorLow;

  PixelInfo
    targetHigh,
    targetLow;

  if (!colorLow_.isValid() || !colorHigh_.isValid())
    throwExceptionExplicit(MagickCore::OptionError,
      "Color argument is invalid");

  colorLow=colorLow_;
  colorHigh=colorHigh_;

  GetPPException;
  (void) QueryColorCompliance(colorLow.c_str(),AllCompliance,&targetLow,
    exceptionInfo);
  (void) QueryColorCompliance(colorHigh.c_str(),AllCompliance,&targetHigh,
    exceptionInfo);
  modifyImage();
  TransparentPaintImageChroma(image(),&targetLow,&targetHigh,TransparentAlpha,
    MagickFalse,exceptionInfo);
  ThrowImageException;
}