static int mailimf_hour_parse(const char * message, size_t length,
			      size_t * indx, int * result)
{
  uint32_t hour;
  int r;

  r = mailimf_number_parse(message, length, indx, &hour);
  if (r != MAILIMF_NO_ERROR)
    return r;

  * result = hour;

  return MAILIMF_NO_ERROR;
}