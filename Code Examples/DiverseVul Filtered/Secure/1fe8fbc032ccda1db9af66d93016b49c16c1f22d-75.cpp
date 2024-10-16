static int mailimf_id_right_parse(const char * message, size_t length,
				  size_t * indx, char ** result)
{
  int r;

  r = mailimf_dot_atom_text_parse(message, length, indx, result);
  switch (r) {
  case MAILIMF_NO_ERROR:
    return MAILIMF_NO_ERROR;
  case MAILIMF_ERROR_PARSE:
    break;
  default:
    return r;
  }

  r = mailimf_no_fold_literal_parse(message, length, indx, result);
  if (r != MAILIMF_NO_ERROR)
    return r;

  return MAILIMF_NO_ERROR;
}