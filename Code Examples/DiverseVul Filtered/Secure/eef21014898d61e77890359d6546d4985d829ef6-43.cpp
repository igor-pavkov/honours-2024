static char *set_ssl_option_unpack_path(const char *arg)
{
  char *opt_var= NULL;
  if (arg)
  {
    char buff[FN_REFLEN + 1];
    unpack_filename(buff, (char *)arg);
    opt_var= my_strdup(buff, MYF(MY_WME));
  }
  return opt_var;
}