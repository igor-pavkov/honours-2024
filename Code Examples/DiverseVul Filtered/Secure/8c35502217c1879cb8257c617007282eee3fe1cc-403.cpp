void Magick::Image::opaque(const Color &opaqueColor_,const Color &penColor_,
  const bool invert_)
{
  std::string
    opaqueColor,
    penColor;

  PixelInfo
    opaque,
    pen;

  if (!opaqueColor_.isValid())
    throwExceptionExplicit(MagickCore::OptionError,
      "Opaque color argument is invalid");

  if (!penColor_.isValid())
    throwExceptionExplicit(MagickCore::OptionError,
      "Pen color argument is invalid");

  modifyImage();
  opaqueColor=opaqueColor_;
  penColor=penColor_;

  GetPPException;
  (void) QueryColorCompliance(opaqueColor.c_str(),AllCompliance,&opaque,
    exceptionInfo);
  (void) QueryColorCompliance(penColor.c_str(),AllCompliance,&pen,
    exceptionInfo);
  OpaquePaintImage(image(),&opaque,&pen,invert_ ? MagickTrue : MagickFalse,
    exceptionInfo);
  ThrowImageException;
}