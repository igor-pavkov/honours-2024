static void _imap_quote_string (char *dest, size_t dlen, const char *src,
                                const char *to_quote)
{
  char *pt;
  const char *s;

  if (!(dest && dlen && src && to_quote))
    return;

  if (dlen < 3)
  {
    *dest = 0;
    return;
  }

  pt = dest;
  s  = src;

  /* save room for pre/post quote-char and trailing null */
  dlen -= 3;

  *pt++ = '"';
  for (; *s && dlen; s++)
  {
    if (strchr (to_quote, *s))
    {
      if (dlen < 2)
        break;
      dlen -= 2;
      *pt++ = '\\';
      *pt++ = *s;
    }
    else
    {
      *pt++ = *s;
      dlen--;
    }
  }
  *pt++ = '"';
  *pt = 0;
}