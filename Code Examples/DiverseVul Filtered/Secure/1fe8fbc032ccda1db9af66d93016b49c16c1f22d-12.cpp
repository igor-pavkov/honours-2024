static int mailimf_no_fold_quote_char_parse(const char * message, size_t length,
					    size_t * indx, char * result)
{
  char ch;
  size_t cur_token;
  int r;

  cur_token = * indx;

#if 0
  r = mailimf_qtext_parse(message, length, &cur_token, &ch);
#endif

  if (cur_token >= length)
    return MAILIMF_ERROR_PARSE;

  if (is_qtext(message[cur_token])) {
    ch = message[cur_token];
    cur_token ++;
  }
  else {
    r = mailimf_quoted_pair_parse(message, length, &cur_token, &ch);
    
    if (r != MAILIMF_NO_ERROR)
      return r;
  }

  * indx = cur_token;
  * result = ch;

  return MAILIMF_NO_ERROR;
}