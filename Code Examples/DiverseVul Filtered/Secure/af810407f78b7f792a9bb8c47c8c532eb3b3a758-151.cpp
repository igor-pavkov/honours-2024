bool Discovered_table_list::add_table(const char *tname, size_t tlen)
{
  /*
    TODO Check with_temps and filter out temp tables.
    Implement the check, when we'll have at least one affected engine (with
    custom discover_table_names() method, that calls add_table() directly).
    Note: avoid comparing the same name twice (here and in add_file).
  */
  if (wild && my_wildcmp(table_alias_charset, tname, tname + tlen, wild, wend,
                         wild_prefix, wild_one, wild_many))
      return 0;

  LEX_CSTRING *name= thd->make_clex_string(tname, tlen);
  if (!name || tables->append(name))
    return 1;
  return 0;
}