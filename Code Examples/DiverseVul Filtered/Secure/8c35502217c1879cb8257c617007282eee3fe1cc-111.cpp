void Magick::Image::modulate(const double brightness_,const double saturation_,
  const double hue_)
{
  char
    modulate[MagickPathExtent + 1];

  FormatLocaleString(modulate,MagickPathExtent,"%3.6f,%3.6f,%3.6f",brightness_,
    saturation_,hue_);

  modifyImage();
  GetPPException;
  ModulateImage(image(),modulate,exceptionInfo);
  ThrowImageException;
}