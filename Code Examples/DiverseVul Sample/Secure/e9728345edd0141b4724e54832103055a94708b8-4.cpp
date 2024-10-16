static inline const char *GetDPXProperty(const Image *image,
  const char *property,ExceptionInfo *exception)
{
  const char
    *value;

  value=GetImageArtifact(image,property);
  if (value != (const char *) NULL)
    return(value);
  return(GetImageProperty(image,property,exception));
}