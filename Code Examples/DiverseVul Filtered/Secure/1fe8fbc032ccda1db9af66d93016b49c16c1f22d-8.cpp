static int mailimf_item_name_parse(const char * message, size_t length,
				   size_t * indx, char ** result)
{
  size_t cur_token;
  size_t begin;
  char * item_name;
  char ch;
  int digit;
  int r;
  int res;

  cur_token = * indx;

  begin = cur_token;

  r = mailimf_alpha_parse(message, length, &cur_token, &ch);
  if (r != MAILIMF_NO_ERROR) {
    res = r;
    goto err;
  }

  while (1) {
    int minus_sign;

    minus_sign = mailimf_minus_parse(message, length, &cur_token);

    r = mailimf_alpha_parse(message, length, &cur_token, &ch);
    if (r == MAILIMF_ERROR_PARSE)
      r = mailimf_digit_parse(message, length, &cur_token, &digit);

    if (r == MAILIMF_NO_ERROR) {
      /* do nothing */
    }
    if (r == MAILIMF_ERROR_PARSE)
      break;
    else if (r != MAILIMF_NO_ERROR) {
      res = r;
      goto err;
    }
  }

  item_name = strndup(message + begin, cur_token - begin);
  if (item_name == NULL) {
    res = MAILIMF_ERROR_MEMORY;
    goto err;
  }

  * indx = cur_token;
  * result = item_name;

  return MAILIMF_NO_ERROR;

 err:
  return res;
}