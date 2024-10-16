static void test_bind_nagative()
{
  MYSQL_STMT *stmt_insert;
  char *query;
  int rc;
  MYSQL_BIND      my_bind[1];
  int32           my_val= 0;
  ulong           my_length= 0L;
  my_bool         my_null= FALSE;
  myheader("test_insert_select");

  rc= mysql_query(mysql, "DROP TABLE IF EXISTS t1");
  myquery(rc);

  rc= mysql_query(mysql, "create temporary table t1 (c1 int unsigned)");
  myquery(rc);

  rc= mysql_query(mysql, "INSERT INTO t1 VALUES (1), (-1)");
  myquery(rc);

  query= (char*)"INSERT INTO t1 VALUES (?)";
  stmt_insert= mysql_simple_prepare(mysql, query);
  check_stmt(stmt_insert);

  /* bind parameters */
  bzero((char*) my_bind, sizeof(my_bind));

  my_bind[0].buffer_type= MYSQL_TYPE_LONG;
  my_bind[0].buffer= (void *)&my_val;
  my_bind[0].length= &my_length;
  my_bind[0].is_null= (char*)&my_null;

  rc= mysql_stmt_bind_param(stmt_insert, my_bind);
  check_execute(stmt_insert, rc);

  my_val= -1;
  rc= mysql_stmt_execute(stmt_insert);
  check_execute(stmt_insert, rc);

  mysql_stmt_close(stmt_insert);
  rc= mysql_query(mysql, "drop table t1");
  myquery(rc);
}