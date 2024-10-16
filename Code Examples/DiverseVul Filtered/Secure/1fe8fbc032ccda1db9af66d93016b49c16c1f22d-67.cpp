static int mailimf_no_fold_literal_parse(const char * message, size_t length,
					 size_t * indx, char ** result)
{
  size_t cur_token;
  size_t begin;
  char ch;
  char * no_fold_literal;
  int r;
  int res;

  begin = cur_token;
  r = mailimf_obracket_parse(message, length, &cur_token);
  if (r != MAILIMF_NO_ERROR) {
    res = r;
    goto err;
  }

  while (1) {
    r = mailimf_no_fold_literal_char_parse(message, length,
					   &cur_token, &ch);
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

  r = mailimf_cbracket_parse(message, length, &cur_token);
  if (r != MAILIMF_NO_ERROR) {
    res = r;
    goto err;
  }

  /*
  no_fold_literal = strndup(message + begin, cur_token - begin);
  */
  no_fold_literal = malloc(cur_token - begin + 1);
  if (no_fold_literal == NULL) {
    res = MAILIMF_NO_ERROR;
    goto err;
  }
  strncpy(no_fold_literal, message + begin, cur_token - begin);
  no_fold_literal[cur_token - begin] = '\0';

  * result = no_fold_literal;
  * indx = cur_token;

  return MAILIMF_NO_ERROR;

 err:
  return res;
}