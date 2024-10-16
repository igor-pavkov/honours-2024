static void test_bug36326()
{
  int rc;

  DBUG_ENTER("test_bug36326");
  myheader("test_bug36326");

  if (! is_query_cache_available())
  {
    fprintf(stdout, "Skipping test_bug36326: Query cache not available.\n");
    DBUG_VOID_RETURN;
  }

  rc= mysql_autocommit(mysql, TRUE);
  myquery(rc);
  rc= mysql_query(mysql, "DROP TABLE IF EXISTS t1");
  myquery(rc);
  rc= mysql_query(mysql, "CREATE  TABLE t1 (a INTEGER)");
  myquery(rc);
  rc= mysql_query(mysql, "INSERT INTO t1 VALUES (1)");
  myquery(rc);
  rc= mysql_query(mysql, "SET GLOBAL query_cache_type = 1");
  myquery(rc);
  rc= mysql_query(mysql, "SET GLOBAL query_cache_size = 1048576");
  myquery(rc);
  DIE_UNLESS(!(mysql->server_status & SERVER_STATUS_IN_TRANS));
  DIE_UNLESS(mysql->server_status & SERVER_STATUS_AUTOCOMMIT);
  rc= mysql_query(mysql, "BEGIN");
  myquery(rc);
  DIE_UNLESS(mysql->server_status & SERVER_STATUS_IN_TRANS);
  rc= mysql_query(mysql, "SELECT * FROM t1");
  myquery(rc);
  rc= my_process_result(mysql);
  DIE_UNLESS(rc == 1);
  rc= mysql_rollback(mysql);
  myquery(rc);
  rc= mysql_query(mysql, "ROLLBACK");
  myquery(rc);
  DIE_UNLESS(!(mysql->server_status & SERVER_STATUS_IN_TRANS));
  rc= mysql_query(mysql, "SELECT * FROM t1");
  myquery(rc);
  DIE_UNLESS(!(mysql->server_status & SERVER_STATUS_IN_TRANS));
  rc= my_process_result(mysql);
  DIE_UNLESS(rc == 1);
  rc= mysql_query(mysql, "DROP TABLE t1");
  myquery(rc);
  rc= mysql_query(mysql, "SET GLOBAL query_cache_size = 0");
  myquery(rc);

  DBUG_VOID_RETURN;
}