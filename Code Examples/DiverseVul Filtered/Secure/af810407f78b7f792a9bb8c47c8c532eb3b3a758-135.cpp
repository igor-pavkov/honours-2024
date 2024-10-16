bool HA_CREATE_INFO::check_conflicting_charset_declarations(CHARSET_INFO *cs)
{
  if ((used_fields & HA_CREATE_USED_DEFAULT_CHARSET) &&
      /* DEFAULT vs explicit, or explicit vs DEFAULT */
      (((default_table_charset == NULL) != (cs == NULL)) ||
      /* Two different explicit character sets */
       (default_table_charset && cs &&
        !my_charset_same(default_table_charset, cs))))
  {
    my_error(ER_CONFLICTING_DECLARATIONS, MYF(0),
             "CHARACTER SET ", default_table_charset ?
                               default_table_charset->csname : "DEFAULT",
             "CHARACTER SET ", cs ? cs->csname : "DEFAULT");
    return true;
  }
  return false;
}