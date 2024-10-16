MagickExport MagickStatusType ParseRegionGeometry(const Image *image,
  const char *geometry,RectangleInfo *region_info,ExceptionInfo *exception)
{
  MagickStatusType
    flags;

  SetGeometry(image,region_info);
  flags=ParseMetaGeometry(geometry,&region_info->x,&region_info->y,
    &region_info->width,&region_info->height);
  if (flags == NoValue)
    (void) ThrowMagickException(exception,GetMagickModule(),OptionError,
      "InvalidGeometry","`%s'",geometry);
  return(flags);
}