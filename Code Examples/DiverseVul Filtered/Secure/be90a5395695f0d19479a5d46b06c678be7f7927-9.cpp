MagickExport ssize_t FormatMagickSize(const MagickSizeType size,
  const MagickBooleanType bi,const char *suffix,const size_t length,
  char *format)
{
  char
    p[MagickPathExtent],
    q[MagickPathExtent];

  const char
    **units;

  double
    bytes,
    extent;

  register ssize_t
    i;

  ssize_t
    count;

  static const char
    *bi_units[] =
    {
      "", "Ki", "Mi", "Gi", "Ti", "Pi", "Ei", "Zi", "Yi", (char *) NULL
    },
    *traditional_units[] =
    {
      "", "K", "M", "G", "T", "P", "E", "Z", "Y", (char *) NULL
    };

  bytes=1000.0;
  units=traditional_units;
  if (bi != MagickFalse)
    {
      bytes=1024.0;
      units=bi_units;
    }
#if defined(_MSC_VER) && (_MSC_VER == 1200)
  extent=(double) ((MagickOffsetType) size);
#else
  extent=(double) size;
#endif
  (void) FormatLocaleString(p,MagickPathExtent,"%.*g",GetMagickPrecision(),
    extent);
  (void) FormatLocaleString(q,MagickPathExtent,"%.20g",extent);
  if (strtod(p,(char **) NULL) == strtod(q,(char **) NULL))
    {
      if (suffix == (const char *) NULL)
        count=FormatLocaleString(format,length,"%.20g%s",extent,units[0]);
      else
        count=FormatLocaleString(format,length,"%.20g%s%s",extent,units[0],
          suffix);
      return(count);
    }
  for (i=0; (extent >= bytes) && (units[i+1] != (const char *) NULL); i++)
    extent/=bytes;
  if (suffix == (const char *) NULL)
    count=FormatLocaleString(format,length,"%.*g%s",GetMagickPrecision(),
      extent,units[i]);
  else
    count=FormatLocaleString(format,length,"%.*g%s%s",GetMagickPrecision(),
      extent,units[i],suffix);
  return(count);
}