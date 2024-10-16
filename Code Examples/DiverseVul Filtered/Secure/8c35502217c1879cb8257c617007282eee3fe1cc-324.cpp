void Magick::Image::transparent(const Color &color_,const bool inverse_)
{
  PixelInfo
    target;

  std::string
    color;

  if (!color_.isValid())
    throwExceptionExplicit(MagickCore::OptionError,
      "Color argument is invalid");

  color=color_;
  GetPPException;
  (void) QueryColorCompliance(color.c_str(),AllCompliance,&target,
    exceptionInfo);
  modifyImage();
  TransparentPaintImage(image(),&target,TransparentAlpha,
    inverse_ == true ? MagickTrue : MagickFalse,exceptionInfo);
  ThrowImageException;
}