static int nntp_date(struct NntpServer *nserv, time_t *now)
{
  if (nserv->hasDATE)
  {
    struct NntpData nntp_data;
    char buf[LONG_STRING];
    struct tm tm;
    memset(&tm, 0, sizeof(tm));

    nntp_data.nserv = nserv;
    nntp_data.group = NULL;
    mutt_str_strfcpy(buf, "DATE\r\n", sizeof(buf));
    if (nntp_query(&nntp_data, buf, sizeof(buf)) < 0)
      return -1;

    if (sscanf(buf, "111 %4d%2d%2d%2d%2d%2d%*s", &tm.tm_year, &tm.tm_mon,
               &tm.tm_mday, &tm.tm_hour, &tm.tm_min, &tm.tm_sec) == 6)
    {
      tm.tm_year -= 1900;
      tm.tm_mon--;
      *now = timegm(&tm);
      if (*now >= 0)
      {
        mutt_debug(1, "server time is %lu\n", *now);
        return 0;
      }
    }
  }
  time(now);
  return 0;
}