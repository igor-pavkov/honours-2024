static int mailimf_domain_parse(const char * message, size_t length,
				size_t * indx,
				char ** result)
{
  int r;

  r = mailimf_dot_atom_parse(message, length, indx, result);
  switch (r) {
  case MAILIMF_NO_ERROR:
    return r;
  case MAILIMF_ERROR_PARSE:
    break;
  default:
    return r;
  }

  r = mailimf_domain_literal_parse(message, length, indx, result);
  if (r != MAILIMF_NO_ERROR)
    return r;

  return MAILIMF_NO_ERROR;
}