void *Magick::Image::getMetacontent(void )
{
  void
    *result;

  result=GetAuthenticMetacontent(image());

  if(!result)
    throwExceptionExplicit(MagickCore::OptionError,
      "Unable to retrieve meta content.");

  return(result);
}