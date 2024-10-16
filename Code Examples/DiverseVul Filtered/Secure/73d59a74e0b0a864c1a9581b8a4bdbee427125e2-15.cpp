static inline size_t WriteChannelSize(const PSDInfo *psd_info,Image *image,
  const signed short channel)
{
  size_t
    count;

  count=(size_t) WriteBlobShort(image,channel);
  count+=SetPSDSize(psd_info,image,0);
  return(count);
}