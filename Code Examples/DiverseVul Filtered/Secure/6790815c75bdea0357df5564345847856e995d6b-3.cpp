MagickExport MagickBooleanType DefineImageOption(ImageInfo *image_info,
  const char *option)
{
  char
    key[MagickPathExtent],
    value[MagickPathExtent];

  register char
    *p;

  assert(image_info != (ImageInfo *) NULL);
  assert(option != (const char *) NULL);
  (void) CopyMagickString(key,option,MagickPathExtent);
  for (p=key; *p != '\0'; p++)
    if (*p == '=')
      break;
  *value='\0';
  if (*p == '=')
    (void) CopyMagickString(value,p+1,MagickPathExtent);
  *p='\0';
  return(SetImageOption(image_info,key,value));
}