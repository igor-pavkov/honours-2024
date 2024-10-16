MagickExport char **StringToList(const char *text)
{
  return(StringToStrings(text,(size_t *) NULL));
}