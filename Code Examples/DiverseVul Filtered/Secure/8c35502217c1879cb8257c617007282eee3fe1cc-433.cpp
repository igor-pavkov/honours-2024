std::string Magick::Image::format(void) const
{
  const MagickInfo 
   *magick_info;

  GetPPException;
  magick_info=GetMagickInfo(constImage()->magick,exceptionInfo);
  ThrowImageException;

  if ((magick_info != 0) && (*magick_info->description != '\0'))
    return(std::string(magick_info->description));

  if (!quiet())
    throwExceptionExplicit(MagickCore::CorruptImageWarning,
      "Unrecognized image magick type");

  return(std::string());
}