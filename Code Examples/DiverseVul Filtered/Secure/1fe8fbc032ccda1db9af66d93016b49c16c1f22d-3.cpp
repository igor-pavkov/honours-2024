static int mailimf_month_parse(const char * message, size_t length,
			       size_t * indx, int * result)
{
  size_t cur_token;
  int month;
  int r;

  cur_token = * indx;

  r = mailimf_cfws_parse(message, length, &cur_token);
  if ((r != MAILIMF_NO_ERROR) && (r != MAILIMF_ERROR_PARSE))
    return r;

  r = mailimf_month_name_parse(message, length, &cur_token, &month);
  if (r != MAILIMF_NO_ERROR)
    return r;

  * result = month;
  * indx = cur_token;

  return MAILIMF_NO_ERROR;
}