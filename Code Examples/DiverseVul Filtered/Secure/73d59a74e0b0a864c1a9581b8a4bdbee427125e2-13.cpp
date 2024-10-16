static inline ssize_t WritePSDSize(const PSDInfo *psd_info,Image *image,
  const MagickSizeType size,const MagickSizeType offset)
{
  MagickSizeType
    current_offset;

  ssize_t
    result;

  current_offset=TellBlob(image);
  SeekBlob(image,offset,SEEK_SET);
  result=SetPSDSize(psd_info, image, size);
  SeekBlob(image,current_offset,SEEK_SET);
  return(result);
}