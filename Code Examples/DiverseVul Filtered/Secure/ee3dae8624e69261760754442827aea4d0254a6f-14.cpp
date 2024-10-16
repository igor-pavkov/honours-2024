MagickExport ImageInfo *AcquireImageInfo(void)
{
  ImageInfo
    *image_info;

  image_info=(ImageInfo *) AcquireCriticalMemory(sizeof(*image_info));
  GetImageInfo(image_info);
  return(image_info);
}