const void *Magick::Image::getConstMetacontent(void) const
{
  const void
    *result;

  result=GetVirtualMetacontent(constImage());

  if(!result)
    throwExceptionExplicit(MagickCore::OptionError,
      "Unable to retrieve meta content.");

  return(result);
}