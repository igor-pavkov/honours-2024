MagickExport MagickStatusType ParsePageGeometry(const Image *image,
  const char *geometry,RectangleInfo *region_info,ExceptionInfo *exception)
{
  MagickStatusType
    flags;

  SetGeometry(image,region_info);
  if (image->page.width != 0)
    region_info->width=image->page.width;
  if (image->page.height != 0)
    region_info->height=image->page.height;
  flags=ParseAbsoluteGeometry(geometry,region_info);
  if (flags == NoValue)
    {
      (void) ThrowMagickException(exception,GetMagickModule(),OptionError,
        "InvalidGeometry","`%s'",geometry);
      return(flags);
    }
  if ((flags & PercentValue) != 0)
    {
      region_info->width=image->columns;
      region_info->height=image->rows;
    }
  flags=ParseMetaGeometry(geometry,&region_info->x,&region_info->y,
    &region_info->width,&region_info->height);
  if ((((flags & WidthValue) != 0) || ((flags & HeightValue) != 0)) &&
      (((flags & PercentValue) != 0) || ((flags & SeparatorValue) == 0)))
    {
      if ((flags & WidthValue) == 0)
        region_info->width=region_info->height;
      if ((flags & HeightValue) == 0)
        region_info->height=region_info->width;
    }
  return(flags);
}