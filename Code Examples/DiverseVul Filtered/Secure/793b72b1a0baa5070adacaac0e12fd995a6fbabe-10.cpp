char **fill_out_embedded_options(PerlIO *stream,
                                 char *options,
                                 int options_type,
                                 int slen, int cnt)
{
  int  ind, len;
  char c;
  char *ptr;
  char **options_list= NULL;

  if (!(options_list= (char **) calloc(cnt, sizeof(char *))))
  {
    PerlIO_printf(stream,
                  "Initialize embedded server. Out of memory \n");
    return NULL;
  }

  ptr= options;
  ind= 0;

  if (options_type == 0)
  {
    /* server_groups list NULL terminated */
    options_list[cnt]= (char *) NULL;
  }

  if (options_type == 1)
  {
    /* first item in server_options list is ignored. fill it with \0 */
    if (!(options_list[0]= calloc(1,sizeof(char))))
      return NULL;

    ind++;
  }

  while ((c= *ptr++))
  {
    slen--;
    if (c == ',' || !slen)
    {
      len= ptr - options;
      if (c == ',')
        len--;
      if (!(options_list[ind]=calloc(len+1,sizeof(char))))
        return NULL;

      strncpy(options_list[ind], options, len);
      ind++;
      options= ptr;
    }
  }
  return options_list;
}