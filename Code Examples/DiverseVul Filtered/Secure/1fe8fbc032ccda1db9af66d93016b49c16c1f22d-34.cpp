static int mailimf_item_value_atom_parse(const char * message, size_t length,
					 size_t * indx, char ** result)
{
  char * atom;
  size_t cur_token;
  int r;

  cur_token = * indx;

  r = mailimf_cfws_parse(message, length, &cur_token);
  if ((r != MAILIMF_NO_ERROR) && (r != MAILIMF_ERROR_PARSE))
    return r;

  r = mailimf_custom_string_parse(message, length, &cur_token,
				  &atom, is_item_value_atext);
  if (r != MAILIMF_NO_ERROR)
    return r;

  r = mailimf_cfws_parse(message, length, &cur_token);
  if ((r != MAILIMF_NO_ERROR) && (r != MAILIMF_ERROR_PARSE))
    return r;

  * indx = cur_token;
  * result = atom;

  return MAILIMF_NO_ERROR;
}