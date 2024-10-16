static ssize_t TIFFWriteCustomStream(unsigned char *data,const size_t count,
  void *user_data)
{
  PhotoshopProfile
    *profile;

  if (count == 0)
    return(0);
  profile=(PhotoshopProfile *) user_data;
  if ((profile->offset+(MagickOffsetType) count) >=
        (MagickOffsetType) profile->extent)
    {
      profile->extent+=count+profile->quantum;
      profile->quantum<<=1;
      SetStringInfoLength(profile->data,profile->extent);
    }
  (void) memcpy(profile->data->datum+profile->offset,data,count);
  profile->offset+=count;
  return(count);
}