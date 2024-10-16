inline ulong escape_string_mysql(THD *thd, char *to, const char *from,
                                 ulong length)
{
    if (!(thd->variables.sql_mode & MODE_NO_BACKSLASH_ESCAPES))
      return (uint)escape_string_for_mysql(system_charset_info, to, 0, from,
                                           length);
    else
      return (uint)escape_quotes_for_mysql(system_charset_info, to, 0, from,
                                           length);
}