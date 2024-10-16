MagickExport char *SanitizeString(const char *source)
{
  char
    *sanitize_source;

  const char
    *q;

  register char
    *p;

  static char
    whitelist[] =
      "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789 "
      "$-_.+!*'(),{}|\\^~[]`\"><#%;/?:@&=";

  sanitize_source=AcquireString(source);
  p=sanitize_source;
  q=sanitize_source+strlen(sanitize_source);
  for (p+=strspn(p,whitelist); p != q; p+=strspn(p,whitelist))
    *p='_';
  return(sanitize_source);
}