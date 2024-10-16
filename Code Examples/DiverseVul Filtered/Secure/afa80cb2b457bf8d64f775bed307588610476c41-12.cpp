check_if_modified(
    cupsd_client_t *con,		/* I - Client connection */
    struct stat    *filestats)		/* I - File information */
{
  const char	*ptr;			/* Pointer into field */
  time_t	date;			/* Time/date value */
  off_t		size;			/* Size/length value */


  size = 0;
  date = 0;
  ptr  = httpGetField(con->http, HTTP_FIELD_IF_MODIFIED_SINCE);

  if (*ptr == '\0')
    return (1);

  cupsdLogClient(con, CUPSD_LOG_DEBUG2, "check_if_modified: filestats=%p(" CUPS_LLFMT ", %d)) If-Modified-Since=\"%s\"", filestats, CUPS_LLCAST filestats->st_size, (int)filestats->st_mtime, ptr);

  while (*ptr != '\0')
  {
    while (isspace(*ptr) || *ptr == ';')
      ptr ++;

    if (_cups_strncasecmp(ptr, "length=", 7) == 0)
    {
      ptr += 7;
      size = strtoll(ptr, NULL, 10);

      while (isdigit(*ptr))
        ptr ++;
    }
    else if (isalpha(*ptr))
    {
      date = httpGetDateTime(ptr);
      while (*ptr != '\0' && *ptr != ';')
        ptr ++;
    }
    else
      ptr ++;
  }

  return ((size != filestats->st_size && size != 0) ||
          (date < filestats->st_mtime && date != 0) ||
	  (size == 0 && date == 0));
}