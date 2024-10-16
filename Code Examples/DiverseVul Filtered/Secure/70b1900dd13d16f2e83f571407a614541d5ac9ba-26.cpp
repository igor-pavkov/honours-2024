static int str2offset(curl_off_t *val, char *str)
{
#if SIZEOF_CURL_OFF_T > 4
  /* Ugly, but without going through a bunch of rigmarole, we don't have the
   * definitions for LLONG_{MIN,MAX} or LONG_LONG_{MIN,MAX}.
   */
#ifndef LLONG_MAX
#if defined(_MSC_VER)
#define LLONG_MAX (curl_off_t)0x7FFFFFFFFFFFFFFFi64
#define LLONG_MIN (curl_off_t)0x8000000000000000i64
#elif defined(_CRAYC)
#define LLONG_MAX (curl_off_t)0x7FFFFFFFFFFFFFFF
#define LLONG_MIN (curl_off_t)0x8000000000000000
#else
#define LLONG_MAX (curl_off_t)0x7FFFFFFFFFFFFFFFLL
#define LLONG_MIN (curl_off_t)0x8000000000000000LL
#endif
#endif

  /* this is a duplicate of the function that is also used in libcurl */
  *val = curlx_strtoofft(str, NULL, 0);

  if ((*val == LLONG_MAX || *val == LLONG_MIN) && ERRNO == ERANGE)
    return 1;
#else
  *val = strtol(str, NULL, 0);
  if ((*val == LONG_MIN || *val == LONG_MAX) && ERRNO == ERANGE)
    return 1;
#endif
  return 0;
}