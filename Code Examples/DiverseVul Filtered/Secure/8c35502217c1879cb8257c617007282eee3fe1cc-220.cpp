void Magick::Image::colorMapSize(const size_t entries_)
{
  if (entries_ >MaxColormapSize)
    throwExceptionExplicit(MagickCore::OptionError,
      "Colormap entries must not exceed MaxColormapSize");

  modifyImage();
  GetPPException;
  (void) AcquireImageColormap(image(),entries_,exceptionInfo);
  ThrowImageException;
}