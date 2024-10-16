static void test_change_user()
{
  char buff[256];
  const char *user_pw= "mysqltest_pw";
  const char *user_no_pw= "mysqltest_no_pw";
  const char *pw= "password";
  const char *db= "mysqltest_user_test_database";
  int rc;
  MYSQL*       conn;

  DBUG_ENTER("test_change_user");
  myheader("test_change_user");

  /* Prepare environment */
  sprintf(buff, "drop database if exists %s", db);
  rc= mysql_query(mysql, buff);
  myquery(rc);

  sprintf(buff, "create database %s", db);
  rc= mysql_query(mysql, buff);
  myquery(rc);

  sprintf(buff,
          "grant select on %s.* to %s@'%%' identified by '%s'",
          db,
          user_pw,
          pw);
  rc= mysql_query(mysql, buff);
  myquery(rc);

  sprintf(buff,
          "grant select on %s.* to %s@'localhost' identified by '%s'",
          db,
          user_pw,
          pw);
  rc= mysql_query(mysql, buff);
  myquery(rc);

  sprintf(buff,
          "grant select on %s.* to %s@'%%'",
          db,
          user_no_pw);
  rc= mysql_query(mysql, buff);
  myquery(rc);

  sprintf(buff,
          "grant select on %s.* to %s@'localhost'",
          db,
          user_no_pw);
  rc= mysql_query(mysql, buff);
  myquery(rc);

  conn= client_connect(0, MYSQL_PROTOCOL_TCP, 0);

  /* Try some combinations */
  rc= mysql_change_user(conn, NULL, NULL, NULL);
  DIE_UNLESS(rc);
  if (! opt_silent)
    printf("Got error (as expected): %s\n", mysql_error(conn));


  rc= mysql_change_user(conn, "", NULL, NULL);
  DIE_UNLESS(rc);
  if (! opt_silent)
    printf("Got error (as expected): %s\n", mysql_error(conn));

  rc= mysql_change_user(conn, "", "", NULL);
  DIE_UNLESS(rc);
  if (! opt_silent)
    printf("Got error (as expected): %s\n", mysql_error(conn));

  mysql_close(conn);
  conn= client_connect(0, MYSQL_PROTOCOL_TCP, 0);

  rc= mysql_change_user(conn, "", "", "");
  DIE_UNLESS(rc);
  if (! opt_silent)
    printf("Got error (as expected): %s\n", mysql_error(conn));

  rc= mysql_change_user(conn, NULL, "", "");
  DIE_UNLESS(rc);
  if (! opt_silent)
    printf("Got error (as expected): %s\n", mysql_error(conn));


  rc= mysql_change_user(conn, NULL, NULL, "");
  DIE_UNLESS(rc);
  if (! opt_silent)
    printf("Got error (as expected): %s\n", mysql_error(conn));

  mysql_close(conn);
  conn= client_connect(0, MYSQL_PROTOCOL_TCP, 0);

  rc= mysql_change_user(conn, "", NULL, "");
  DIE_UNLESS(rc);
  if (! opt_silent)
    printf("Got error (as expected): %s\n", mysql_error(conn));

  rc= mysql_change_user(conn, user_pw, NULL, "");
  DIE_UNLESS(rc);
  if (! opt_silent)
    printf("Got error (as expected): %s\n", mysql_error(conn));

  rc= mysql_change_user(conn, user_pw, "", "");
  DIE_UNLESS(rc);
  if (! opt_silent)
    printf("Got error (as expected): %s\n", mysql_error(conn));

  mysql_close(conn);
  conn= client_connect(0, MYSQL_PROTOCOL_TCP, 0);

  rc= mysql_change_user(conn, user_pw, "", NULL);
  DIE_UNLESS(rc);
  if (! opt_silent)
    printf("Got error (as expected): %s\n", mysql_error(conn));

  rc= mysql_change_user(conn, user_pw, NULL, NULL);
  DIE_UNLESS(rc);
  if (! opt_silent)
    printf("Got error (as expected): %s\n", mysql_error(conn));

  rc= mysql_change_user(conn, user_pw, "", db);
  DIE_UNLESS(rc);
  if (! opt_silent)
    printf("Got error (as expected): %s\n", mysql_error(conn));

  mysql_close(conn);
  conn= client_connect(0, MYSQL_PROTOCOL_TCP, 0);

  rc= mysql_change_user(conn, user_pw, NULL, db);
  DIE_UNLESS(rc);
  if (! opt_silent)
    printf("Got error (as expected): %s\n", mysql_error(conn));

  rc= mysql_change_user(conn, user_pw, pw, db);
  myquery(rc);

  rc= mysql_change_user(conn, user_pw, pw, NULL);
  myquery(rc);

  rc= mysql_change_user(conn, user_pw, pw, "");
  myquery(rc);

  rc= mysql_change_user(conn, user_no_pw, pw, db);
  DIE_UNLESS(rc);
  if (! opt_silent)
    printf("Got error (as expected): %s\n", mysql_error(conn));

  rc= mysql_change_user(conn, user_no_pw, pw, "");
  DIE_UNLESS(rc);
  if (! opt_silent)
    printf("Got error (as expected): %s\n", mysql_error(conn));

  mysql_close(conn);
  conn= client_connect(0, MYSQL_PROTOCOL_TCP, 0);

  rc= mysql_change_user(conn, user_no_pw, pw, NULL);
  DIE_UNLESS(rc);
  if (! opt_silent)
    printf("Got error (as expected): %s\n", mysql_error(conn));

  rc= mysql_change_user(conn, user_no_pw, "", NULL);
  myquery(rc);

  rc= mysql_change_user(conn, user_no_pw, "", "");
  myquery(rc);

  rc= mysql_change_user(conn, user_no_pw, "", db);
  myquery(rc);

  rc= mysql_change_user(conn, user_no_pw, NULL, db);
  myquery(rc);

  rc= mysql_change_user(conn, "", pw, db);
  DIE_UNLESS(rc);
  if (! opt_silent)
    printf("Got error (as expected): %s\n", mysql_error(conn));

  rc= mysql_change_user(conn, "", pw, "");
  DIE_UNLESS(rc);
  if (! opt_silent)
    printf("Got error (as expected): %s\n", mysql_error(conn));

  mysql_close(conn);
  conn= client_connect(0, MYSQL_PROTOCOL_TCP, 0);

  rc= mysql_change_user(conn, "", pw, NULL);
  DIE_UNLESS(rc);
  if (! opt_silent)
    printf("Got error (as expected): %s\n", mysql_error(conn));

  rc= mysql_change_user(conn, NULL, pw, NULL);
  DIE_UNLESS(rc);
  if (! opt_silent)
    printf("Got error (as expected): %s\n", mysql_error(conn));

  rc= mysql_change_user(conn, NULL, NULL, db);
  DIE_UNLESS(rc);
  if (! opt_silent)
    printf("Got error (as expected): %s\n", mysql_error(conn));

  mysql_close(conn);
  conn= client_connect(0, MYSQL_PROTOCOL_TCP, 0);

  rc= mysql_change_user(conn, NULL, "", db);
  DIE_UNLESS(rc);
  if (! opt_silent)
    printf("Got error (as expected): %s\n", mysql_error(conn));

  rc= mysql_change_user(conn, "", "", db);
  DIE_UNLESS(rc);
  if (! opt_silent)
    printf("Got error (as expected): %s\n", mysql_error(conn));

  /* Cleanup the environment */

  mysql_change_user(conn, opt_user, opt_password, current_db);

  mysql_close(conn);

  sprintf(buff, "drop database %s", db);
  rc= mysql_query(mysql, buff);
  myquery(rc);

  sprintf(buff, "drop user %s@'%%'", user_pw);
  rc= mysql_query(mysql, buff);
  myquery(rc);

  sprintf(buff, "drop user %s@'%%'", user_no_pw);
  rc= mysql_query(mysql, buff);
  myquery(rc);

  sprintf(buff, "drop user %s@'localhost'", user_pw);
  rc= mysql_query(mysql, buff);
  myquery(rc);

  sprintf(buff, "drop user %s@'localhost'", user_no_pw);
  rc= mysql_query(mysql, buff);
  myquery(rc);

  DBUG_VOID_RETURN;
}