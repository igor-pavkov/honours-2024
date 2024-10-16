static void free_fixed_hostname(struct hostname *host)
{
#if defined(USE_LIBIDN)
  if(host->encalloc) {
    idn_free(host->encalloc); /* must be freed with idn_free() since this was
                                 allocated by libidn */
    host->encalloc = NULL;
  }
#elif defined(USE_WIN32_IDN)
  free(host->encalloc); /* must be freed withidn_free() since this was
                           allocated by curl_win32_idn_to_ascii */
  host->encalloc = NULL;
#else
  (void)host;
#endif
}