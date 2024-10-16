static void test_bug42373()
{
  int rc;
  MYSQL con;
  MYSQL_STMT *stmt;

  DBUG_ENTER("test_bug42373");
  myheader("test_42373");

  rc= mysql_query(mysql, "DROP PROCEDURE IF EXISTS p1");
  myquery(rc);

  rc= mysql_query(mysql, "CREATE PROCEDURE p1()"
                         "  BEGIN"
                         "  SELECT 1;"
                         "  INSERT INTO t1 VALUES (2);"
                         "END;");
  myquery(rc);

  rc= mysql_query(mysql, "DROP TABLE IF EXISTS t1");
  myquery(rc);

  rc= mysql_query(mysql, "CREATE TABLE t1 (a INT)");
  myquery(rc);

  /* Try with a stored procedure. */
  DIE_UNLESS(mysql_client_init(&con));

  mysql_options(&con, MYSQL_INIT_COMMAND, "CALL p1()");

  DIE_UNLESS(mysql_real_connect(&con, opt_host, opt_user, opt_password,
                                current_db, opt_port, opt_unix_socket,
                                CLIENT_MULTI_STATEMENTS|CLIENT_MULTI_RESULTS));

  stmt= mysql_simple_prepare(&con, "SELECT a FROM t1");
  check_stmt(stmt);

  rc= mysql_stmt_execute(stmt);
  check_execute(stmt, rc);

  rc= my_process_stmt_result(stmt);
  DIE_UNLESS(rc == 1);

  mysql_stmt_close(stmt);

  /* Now try with a multi-statement. */
  DIE_UNLESS(mysql_client_init(&con));

  mysql_options(&con, MYSQL_INIT_COMMAND,
                "SELECT 3; INSERT INTO t1 VALUES (4)");

  DIE_UNLESS(mysql_real_connect(&con, opt_host, opt_user, opt_password,
                                current_db, opt_port, opt_unix_socket,
                                CLIENT_MULTI_STATEMENTS|CLIENT_MULTI_RESULTS));

  stmt= mysql_simple_prepare(&con, "SELECT a FROM t1");
  check_stmt(stmt);

  rc= mysql_stmt_execute(stmt);
  check_execute(stmt, rc);

  rc= my_process_stmt_result(stmt);
  DIE_UNLESS(rc == 2);

  mysql_stmt_close(stmt);
  mysql_close(&con);

  rc= mysql_query(mysql, "DROP TABLE t1");
  myquery(rc);

  rc= mysql_query(mysql, "DROP PROCEDURE p1");
  myquery(rc);

  DBUG_VOID_RETURN;
}