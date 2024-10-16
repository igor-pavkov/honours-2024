Magick::Blob Magick::Image::exifProfile(void) const
{
  const StringInfo 
    *exif_profile;

  exif_profile=GetImageProfile(constImage(),"exif");
  if (exif_profile == (StringInfo *) NULL)
    return(Blob());
  return(Blob(GetStringInfoDatum(exif_profile),
    GetStringInfoLength(exif_profile)));
} 