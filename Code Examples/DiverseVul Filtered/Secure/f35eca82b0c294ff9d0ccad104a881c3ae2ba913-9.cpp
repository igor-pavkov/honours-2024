MagickExport MagickBooleanType IsGeometry(const char *geometry)
{
  GeometryInfo
    geometry_info;

  MagickStatusType
    flags;

  if (geometry == (const char *) NULL)
    return(MagickFalse);
  flags=ParseGeometry(geometry,&geometry_info);
  return(flags != NoValue ? MagickTrue : MagickFalse);
}