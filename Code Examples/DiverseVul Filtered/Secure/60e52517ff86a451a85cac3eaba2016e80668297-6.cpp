static MagickOffsetType TIFFTellCustomStream(void *user_data)
{
  PhotoshopProfile
    *profile;

  profile=(PhotoshopProfile *) user_data;
  return(profile->offset);
}