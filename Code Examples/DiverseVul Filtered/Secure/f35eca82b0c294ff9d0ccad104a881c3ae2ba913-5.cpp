MagickExport MagickBooleanType IsSceneGeometry(const char *geometry,
  const MagickBooleanType pedantic)
{
  char
    *p;

  double
    value;

  if (geometry == (const char *) NULL)
    return(MagickFalse);
  p=(char *) geometry;
  value=StringToDouble(geometry,&p);
  (void) value;
  if (p == geometry)
    return(MagickFalse);
  if (strspn(geometry,"0123456789-, ") != strlen(geometry))
    return(MagickFalse);
  if ((pedantic != MagickFalse) && (strchr(geometry,',') != (char *) NULL))
    return(MagickFalse);
  return(MagickTrue);
}