static CURLcode setstropt(char **charp, const char *s)
{
  /* Release the previous storage at `charp' and replace by a dynamic storage
     copy of `s'. Return CURLE_OK or CURLE_OUT_OF_MEMORY. */

  Curl_safefree(*charp);

  if(s) {
    char *str = strdup(s);

    if(!str)
      return CURLE_OUT_OF_MEMORY;

    *charp = str;
  }

  return CURLE_OK;
}