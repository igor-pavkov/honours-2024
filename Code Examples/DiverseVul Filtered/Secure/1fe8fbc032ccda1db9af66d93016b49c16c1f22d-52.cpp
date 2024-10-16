static int mailimf_minute_parse(const char * message, size_t length,
				size_t * indx, int * result)
{
  uint32_t minute;
  int r;

  r = mailimf_number_parse(message, length, indx, &minute);
  if (r != MAILIMF_NO_ERROR)
    return r;

  * result = minute;

  return MAILIMF_NO_ERROR;
}