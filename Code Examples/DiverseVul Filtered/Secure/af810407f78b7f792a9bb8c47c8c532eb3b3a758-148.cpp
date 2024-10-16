bool Discovered_table_list::add_file(const char *fname)
{
  bool is_temp= strncmp(fname, STRING_WITH_LEN(tmp_file_prefix)) == 0;

  if (is_temp && !with_temps)
    return 0;

  char tname[SAFE_NAME_LEN + 1];
  size_t tlen= filename_to_tablename(fname, tname, sizeof(tname), is_temp);
  return add_table(tname, tlen);
}