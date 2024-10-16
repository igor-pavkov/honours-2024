static int mailimf_no_fold_quote_parse(const char * message, size_t length,
				       size_t * indx, char ** result)
{
  size_t cur_token;
  size_t begin;
  char ch;
  char * no_fold_quote;
  int r;
  int res;

  begin = cur_token;
  r = mailimf_dquote_parse(message, length, &cur_token);
  if (r != MAILIMF_NO_ERROR) {
    res = r;
    goto err;
  }

  while (1) {
    r = mailimf_no_fold_quote_char_parse(message, length, &cur_token, &ch);
    if (r == MAILIMF_NO_ERROR) {
      /* do nothing */
    }
    else if (r == MAILIMF_ERROR_PARSE)
      break;
    else {
      res = r;
      goto err;
    }
  }

  r = mailimf_dquote_parse(message, length, &cur_token);
  if (r != MAILIMF_NO_ERROR) {
    res = r;
    goto err;
  }

  /*  no_fold_quote = strndup(message + begin, cur_token - begin); */
  no_fold_quote = malloc(cur_token - begin + 1);
  if (no_fold_quote == NULL) {
    res = MAILIMF_ERROR_MEMORY;
    goto err;
  }
  strncpy(no_fold_quote, message + begin, cur_token - begin);
  no_fold_quote[cur_token - begin] = '\0';

  * result = no_fold_quote;
  * indx = cur_token;

  return MAILIMF_NO_ERROR;

 err:
  return res;
}