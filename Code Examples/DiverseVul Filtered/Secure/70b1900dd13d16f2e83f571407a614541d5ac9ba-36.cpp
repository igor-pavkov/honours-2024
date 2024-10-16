CURLcode _my_setopt(CURL *curl, const char *name, CURLoption tag, ...)
{
  va_list arg;
  CURLcode ret;
  char *bufp;
  char value[256];
  bool remark=FALSE;

  va_start(arg, tag);

  if(tag < CURLOPTTYPE_OBJECTPOINT) {
    long lval = va_arg(arg, long);
    snprintf(value, sizeof(value), "%ld", lval);
    ret = curl_easy_setopt(curl, tag, lval);

  }
  else if(tag < CURLOPTTYPE_OFF_T) {
    void *pval = va_arg(arg, void *);
    unsigned char *ptr = (unsigned char *)pval;

    /* function pointers are never printable */
    if (tag >= CURLOPTTYPE_FUNCTIONPOINT) {
      if (pval) {
        snprintf(value, sizeof(value), "%p", pval);
        remark = TRUE;
      }
      else
        strcpy(value, "NULL");
    }
    /* attempt to figure out if it is a string (since the tag numerical doesn't
       offer this info) and then output it as a string if so */
    else if(pval && isgraph(ptr[0]) && isgraph(ptr[1]) && isgraph(ptr[2]))
      snprintf(value, sizeof(value), "\"%s\"", (char *)ptr);
    else if(pval) {
      snprintf(value, sizeof(value), "%p", pval);
      remark = TRUE;
    }
    else {
      strcpy(value, "NULL"); /* value fits more than 5 bytes */
    }
    ret = curl_easy_setopt(curl, tag, pval);

  }
  else {
    curl_off_t oval = va_arg(arg, curl_off_t);
    snprintf(value, sizeof(value), "(curl_off_t)%Od", oval);
    ret = curl_easy_setopt(curl, tag, oval);
  }

  bufp = curl_maprintf("%scurl_easy_setopt(hnd, %s, %s);%s",
                       remark?"/* ":"", name, value,
                       remark?" [REMARK] */":"");

  if (!bufp || !curl_slist_append(easycode, bufp))
    ret = CURLE_OUT_OF_MEMORY;
  if (bufp)
    curl_free(bufp);
  va_end(arg);

  return ret;
}