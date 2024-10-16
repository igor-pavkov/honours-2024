static inline ssize_t SetPSDSize(const PSDInfo *psd_info,Image *image,
  const MagickSizeType size)
{
  if (psd_info->version == 1)
    return(WriteBlobLong(image,(unsigned int) size));
  return(WriteBlobLongLong(image,size));
}