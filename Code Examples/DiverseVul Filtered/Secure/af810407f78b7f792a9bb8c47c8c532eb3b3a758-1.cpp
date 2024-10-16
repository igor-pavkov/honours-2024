const char *get_canonical_filename(handler *file, const char *path,
                                   char *tmp_path)
{
  uint i;
  if (lower_case_table_names != 2 || (file->ha_table_flags() & HA_FILE_BASED))
    return path;

  for (i= 0; i <= mysql_tmpdir_list.max; i++)
  {
    if (is_prefix(path, mysql_tmpdir_list.list[i]))
      return path;
  }

  /* Ensure that table handler get path in lower case */
  if (tmp_path != path)
    strmov(tmp_path, path);

  /*
    we only should turn into lowercase database/table part
    so start the process after homedirectory
  */
  my_casedn_str(files_charset_info, tmp_path + mysql_data_home_len);
  return tmp_path;
}