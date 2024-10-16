static int mailimf_domain_literal_parse(const char * message, size_t length,
					size_t * indx, char ** result)
{
  size_t cur_token;
  int len;
  int begin;
  char * domain_literal;
  int r;

  cur_token = * indx;

  r = mailimf_cfws_parse(message, length, &cur_token);
  if ((r != MAILIMF_NO_ERROR) && (r != MAILIMF_ERROR_PARSE))
    return r;

  begin = cur_token;
  r = mailimf_obracket_parse(message, length, &cur_token);
  if (r != MAILIMF_NO_ERROR)
    return r;

  while (1) {
    r = mailimf_domain_literal_fws_dcontent_parse(message, length,
						  &cur_token);
    if (r == MAILIMF_NO_ERROR) {
      /* do nothing */
    }
    else if (r == MAILIMF_ERROR_PARSE)
      break;
    else
      return r;
  }

  r = mailimf_fws_parse(message, length, &cur_token);
  if ((r != MAILIMF_NO_ERROR) && (r != MAILIMF_ERROR_PARSE))
    return r;

  r = mailimf_cbracket_parse(message, length, &cur_token);
  if (r != MAILIMF_NO_ERROR)
    return r;

  len = cur_token - begin;

  domain_literal = malloc(len + 1);
  if (domain_literal == NULL)
    return MAILIMF_ERROR_MEMORY;
  strncpy(domain_literal, message + begin, len);
  domain_literal[len] = '\0';

  * result = domain_literal;
  * indx = cur_token;

  return MAILIMF_NO_ERROR;
}