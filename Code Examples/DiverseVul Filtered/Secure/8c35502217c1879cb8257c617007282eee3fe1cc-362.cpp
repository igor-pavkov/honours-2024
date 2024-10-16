Magick::Blob Magick::Image::profile(const std::string name_) const
{
  const StringInfo
    *profile;

  profile=GetImageProfile(constImage(),name_.c_str());

  if (profile == (StringInfo *) NULL)
    return(Blob());
  return(Blob((void*) GetStringInfoDatum(profile),GetStringInfoLength(
    profile)));
}