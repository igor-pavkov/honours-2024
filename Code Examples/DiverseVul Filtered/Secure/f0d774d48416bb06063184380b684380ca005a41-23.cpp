int mysql_init_character_set(MYSQL *mysql)
{
  /* Set character set */
  if (!mysql->options.charset_name)
  {
    if (!(mysql->options.charset_name= 
       my_strdup(MYSQL_DEFAULT_CHARSET_NAME,MYF(MY_WME))))
      return 1;
  }
  else if (!strcmp(mysql->options.charset_name,
                   MYSQL_AUTODETECT_CHARSET_NAME) &&
            mysql_autodetect_character_set(mysql))
    return 1;

  mysql_set_character_set_with_default_collation(mysql);

  if (!mysql->charset)
  {
    if (mysql->options.charset_dir)
      set_mysql_extended_error(mysql, CR_CANT_READ_CHARSET, unknown_sqlstate,
                               ER(CR_CANT_READ_CHARSET),
                               mysql->options.charset_name,
                               mysql->options.charset_dir);
    else
    {
      char cs_dir_name[FN_REFLEN];
      get_charsets_dir(cs_dir_name);
      set_mysql_extended_error(mysql, CR_CANT_READ_CHARSET, unknown_sqlstate,
                               ER(CR_CANT_READ_CHARSET),
                               mysql->options.charset_name,
                               cs_dir_name);
    }
    return 1;
  }
  return 0;
}