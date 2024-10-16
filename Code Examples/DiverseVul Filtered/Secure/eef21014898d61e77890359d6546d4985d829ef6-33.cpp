static void test_bug58036()
{
  MYSQL *conn;
  DBUG_ENTER("test_bug58036");
  myheader("test_bug58036");

  /* Part1: try to connect with ucs2 client character set */
  conn= mysql_client_init(NULL);
  mysql_options(conn, MYSQL_SET_CHARSET_NAME, "ucs2");
  if (mysql_real_connect(conn, opt_host, opt_user,
                         opt_password,  opt_db ? opt_db : "test",
                         opt_port, opt_unix_socket, 0))
  {
    if (!opt_silent)
      printf("mysql_real_connect() succeeded (failure expected)\n");
    mysql_close(conn);
    DIE("");
  }

  if (!opt_silent)
    printf("Got mysql_real_connect() error (expected): %s (%d)\n",
           mysql_error(conn), mysql_errno(conn));  
  DIE_UNLESS(mysql_errno(conn) == ER_WRONG_VALUE_FOR_VAR ||
             mysql_errno(conn)== CR_CANT_READ_CHARSET);
  mysql_close(conn);


  /*
    Part2:
    - connect with latin1
    - then change client character set to ucs2
    - then try mysql_change_user()
  */
  conn= mysql_client_init(NULL);
  mysql_options(conn, MYSQL_SET_CHARSET_NAME, "latin1");
  if (!mysql_real_connect(conn, opt_host, opt_user,
                         opt_password, opt_db ? opt_db : "test",
                         opt_port, opt_unix_socket, 0))
  {
    if (!opt_silent)
      printf("mysql_real_connect() failed: %s (%d)\n",
             mysql_error(conn), mysql_errno(conn));
    mysql_close(conn);
    DIE("");
  }

  mysql_options(conn, MYSQL_SET_CHARSET_NAME, "ucs2");
  if (!mysql_change_user(conn, opt_user, opt_password, NULL))
  {
    if (!opt_silent)
      printf("mysql_change_user() succedded, error expected!");
    mysql_close(conn);
    DIE("");
  }

  if (!opt_silent)
    printf("Got mysql_change_user() error (expected): %s (%d)\n",
           mysql_error(conn), mysql_errno(conn));
  mysql_close(conn);

  DBUG_VOID_RETURN;
}