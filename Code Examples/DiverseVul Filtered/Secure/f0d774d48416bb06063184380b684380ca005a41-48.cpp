int cli_read_change_user_result(MYSQL *mysql)
{
  return cli_safe_read(mysql);
}