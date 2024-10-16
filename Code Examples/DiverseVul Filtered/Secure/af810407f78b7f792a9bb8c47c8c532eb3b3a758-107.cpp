int handler::delete_table(const char *name)
{
  int saved_error= 0;
  int error= 0;
  int enoent_or_zero;

  if (ht->discover_table)
    enoent_or_zero= 0; // the table may not exist in the engine, it's ok
  else
    enoent_or_zero= ENOENT;  // the first file of bas_ext() *must* exist

  for (const char **ext=bas_ext(); *ext ; ext++)
  {
    if (mysql_file_delete_with_symlink(key_file_misc, name, *ext, 0))
    {
      if (my_errno != ENOENT)
      {
        /*
          If error on the first existing file, return the error.
          Otherwise delete as much as possible.
        */
        if (enoent_or_zero)
          return my_errno;
	saved_error= my_errno;
      }
    }
    else
      enoent_or_zero= 0;                        // No error for ENOENT
    error= enoent_or_zero;
  }
  return saved_error ? saved_error : error;
}