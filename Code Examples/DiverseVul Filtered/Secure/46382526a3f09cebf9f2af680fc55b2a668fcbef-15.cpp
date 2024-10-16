static size_t convertHTMLcodes(char *s, const size_t len)
{
  int
    value;

  if ((len == 0) || (s == (char*) NULL) || (*s=='\0'))
    return(0);
  if ((len > 3) && (s[1] == '#') && (strchr(s,';') != (char *) NULL) &&
      (sscanf(s,"&#%d;",&value) == 1))
    {
      size_t o = 3;
      while (s[o] != ';')
      {
        o++;
        if (o > 5)
          break;
      }
      if (o < 6)
        (void) memmove(s+1,s+1+o,strlen(s+1+o)+1);
      *s=value;
      return(o);
    }
  else
    {
      int
        i,
        codes;

      codes=sizeof(html_codes)/sizeof(html_code);
      for (i=0; i < codes; i++)
      {
        if (html_codes[i].len <= (ssize_t) len)
          if (stringnicmp(s, html_codes[i].code,(size_t) (html_codes[i].len)) == 0)
            {
              (void) memmove(s+1,s+html_codes[i].len,
                strlen(s+html_codes[i].len)+1);
              *s=html_codes[i].val;
              return(html_codes[i].len-1);
            }
      }
    }
  return(0);
}