void httpd_main(void)
{
  if (toys.optc && chdir(*toys.optargs))
    return error_time(500, "Internal Error");
  // inetd only at the moment
  handle(0, 1);
}