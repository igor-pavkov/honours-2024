static const char *tftp_option_get(const char *buf, size_t len,
                                   const char **option, const char **value)
{
  size_t loc;

  loc = Curl_strnlen(buf, len);
  loc++; /* NULL term */

  if(loc >= len)
    return NULL;
  *option = buf;

  loc += Curl_strnlen(buf + loc, len-loc);
  loc++; /* NULL term */

  if(loc > len)
    return NULL;
  *value = &buf[strlen(*option) + 1];

  return &buf[loc];
}