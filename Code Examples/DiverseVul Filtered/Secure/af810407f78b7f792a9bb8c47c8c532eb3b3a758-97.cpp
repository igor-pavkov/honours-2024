int handler::rename_table(const char * from, const char * to)
{
  int error= 0;
  const char **ext, **start_ext;
  start_ext= bas_ext();
  for (ext= start_ext; *ext ; ext++)
  {
    if (unlikely(rename_file_ext(from, to, *ext)))
    {
      if ((error=my_errno) != ENOENT)
	break;
      error= 0;
    }
  }
  if (unlikely(error))
  {
    /* Try to revert the rename. Ignore errors. */
    for (; ext >= start_ext; ext--)
      rename_file_ext(to, from, *ext);
  }
  return error;
}