static int mailimf_dot_atom_parse(const char * message, size_t length,
				  size_t * indx, char ** result)
{
  return mailimf_atom_parse(message, length, indx, result);
}