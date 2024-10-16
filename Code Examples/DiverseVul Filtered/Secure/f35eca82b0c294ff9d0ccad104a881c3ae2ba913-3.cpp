MagickExport MagickStatusType ParseGravityGeometry(const Image *image,
  const char *geometry,RectangleInfo *region_info,ExceptionInfo *exception)
{
  MagickStatusType
    flags;

  size_t
    height,
    width;

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
      GeometryInfo
        geometry_info;

      MagickStatusType
        status;

      PointInfo
        scale;

      /*
        Geometry is a percentage of the image size, not canvas size
      */
      if (image->gravity != UndefinedGravity)
        flags|=XValue | YValue;
      status=ParseGeometry(geometry,&geometry_info);
      scale.x=geometry_info.rho;
      if ((status & RhoValue) == 0)
        scale.x=100.0;
      scale.y=geometry_info.sigma;
      if ((status & SigmaValue) == 0)
        scale.y=scale.x;
      region_info->width=(size_t) floor((scale.x*image->columns/100.0)+0.5);
      region_info->height=(size_t) floor((scale.y*image->rows/100.0)+0.5);
    }
  if ((flags & AspectRatioValue) != 0)
    {
      double
        geometry_ratio,
        image_ratio;

      GeometryInfo
        geometry_info;

      /*
        Geometry is a relative to image size and aspect ratio.
      */
      if (image->gravity != UndefinedGravity)
        flags|=XValue | YValue;
      (void) ParseGeometry(geometry,&geometry_info);
      geometry_ratio=geometry_info.rho;
      image_ratio=(double) image->columns/image->rows;
      if (geometry_ratio >= image_ratio)
        {
          region_info->width=image->columns;
          region_info->height=(size_t) floor((double) (image->rows*image_ratio/
            geometry_ratio)+0.5);
        }
      else
        {
          region_info->width=(size_t) floor((double) (image->columns*
            geometry_ratio/image_ratio)+0.5);
          region_info->height=image->rows;
        }
    }
  /*
    Adjust offset according to gravity setting.
  */
  width=region_info->width;
  height=region_info->height;
  if (width == 0)
    region_info->width=image->page.width | image->columns;
  if (height == 0)
    region_info->height=image->page.height | image->rows;
  GravityAdjustGeometry(image->columns,image->rows,image->gravity,region_info);
  region_info->width=width;
  region_info->height=height;
  return(flags);
}