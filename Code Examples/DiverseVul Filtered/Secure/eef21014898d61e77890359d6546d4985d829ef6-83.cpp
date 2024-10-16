static void test_bug6046()
{
  MYSQL_STMT *stmt;
  const char *stmt_text;
  int rc;
  short b= 1;
  MYSQL_BIND my_bind[1];

  myheader("test_bug6046");

  stmt_text= "DROP TABLE IF EXISTS t1";
  rc= mysql_real_query(mysql, stmt_text, strlen(stmt_text));
  myquery(rc);
  stmt_text= "CREATE TABLE t1 (a int, b int)";
  rc= mysql_real_query(mysql, stmt_text, strlen(stmt_text));
  myquery(rc);
  stmt_text= "INSERT INTO t1 VALUES (1,1),(2,2),(3,1),(4,2)";
  rc= mysql_real_query(mysql, stmt_text, strlen(stmt_text));
  myquery(rc);

  stmt= mysql_stmt_init(mysql);

  stmt_text= "SELECT t1.a FROM t1 NATURAL JOIN t1 as X1 "
             "WHERE t1.b > ? ORDER BY t1.a";

  rc= mysql_stmt_prepare(stmt, stmt_text, strlen(stmt_text));
  check_execute(stmt, rc);

  b= 1;
  bzero((char*) my_bind, sizeof(my_bind));
  my_bind[0].buffer= &b;
  my_bind[0].buffer_type= MYSQL_TYPE_SHORT;

  mysql_stmt_bind_param(stmt, my_bind);

  rc= mysql_stmt_execute(stmt);
  check_execute(stmt, rc);
  mysql_stmt_store_result(stmt);

  rc= mysql_stmt_execute(stmt);
  check_execute(stmt, rc);

  mysql_stmt_close(stmt);
}