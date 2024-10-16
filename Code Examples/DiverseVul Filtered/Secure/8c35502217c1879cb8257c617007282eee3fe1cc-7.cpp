Magick::Blob Magick::Image::iccColorProfile(void) const
{
  const StringInfo
    *color_profile;

  color_profile=GetImageProfile(constImage(),"icc");
  if (color_profile == (StringInfo *) NULL)
    return(Blob());
  return(Blob(GetStringInfoDatum(color_profile),GetStringInfoLength(
    color_profile)));
}