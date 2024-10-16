static inline const char *GetCINProperty(const ImageInfo *image_info,
  const Image *image,const char *property)
{
  const char
    *value;

  value=GetImageOption(image_info,property);
  if (value != (const char *) NULL)
    return(value);
  return(GetImageProperty(image,property));
}