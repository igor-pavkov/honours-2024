static MagickBooleanType IsTXT(const unsigned char *magick,const size_t length)
{
  char
    colorspace[MagickPathExtent];

  ssize_t
    count;

  unsigned long
    columns,
    depth,
    rows;

  if (length < 40)
    return(MagickFalse);
  if (LocaleNCompare((const char *) magick,MagickTXTID,
        strlen(MagickTXTID)) != 0)
    return(MagickFalse);
  count=(ssize_t) sscanf((const char *) magick+32,"%lu,%lu,%lu,%32s",&columns,
    &rows,&depth,colorspace);
  if (count != 4)
    return(MagickFalse);
  return(MagickTrue);
}