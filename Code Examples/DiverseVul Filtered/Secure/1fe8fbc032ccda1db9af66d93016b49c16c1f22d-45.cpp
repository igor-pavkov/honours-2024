static int mailimf_greater_parse(const char * message, size_t length,
				      size_t * indx)
{
  return mailimf_unstrict_char_parse(message, length, indx, '>');
}