Magick::Blob Magick::Image::iptcProfile(void) const
{
  const StringInfo
    *iptc_profile;

  iptc_profile=GetImageProfile(constImage(),"iptc");
  if (iptc_profile == (StringInfo *) NULL)
    return(Blob());
  return(Blob(GetStringInfoDatum(iptc_profile),GetStringInfoLength(
    iptc_profile)));
}