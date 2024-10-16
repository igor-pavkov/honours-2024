static void test_view_star()
{
  MYSQL_STMT *stmt;
  int rc, i;
  MYSQL_BIND      my_bind[8];
  char            parms[8][100];
  ulong           length[8];
  const char *query= "SELECT * FROM vt1 WHERE a IN (?,?)";

  myheader("test_view_star");

  rc= mysql_query(mysql, "DROP TABLE IF EXISTS t1, vt1");
  myquery(rc);
  rc= mysql_query(mysql, "DROP VIEW IF EXISTS t1, vt1");
  myquery(rc);
  rc= mysql_query(mysql, "CREATE TABLE t1 (a int)");
  myquery(rc);
  rc= mysql_query(mysql, "CREATE VIEW vt1 AS SELECT a FROM t1");
  myquery(rc);
  bzero((char*) my_bind, sizeof(my_bind));
  for (i= 0; i < 2; i++) {
    sprintf((char *)&parms[i], "%d", i);
    my_bind[i].buffer_type = MYSQL_TYPE_VAR_STRING;
    my_bind[i].buffer = (char *)&parms[i];
    my_bind[i].buffer_length = 100;
    my_bind[i].is_null = 0;
    my_bind[i].length = &length[i];
    length[i] = 1;
  }

  stmt= mysql_stmt_init(mysql);
  rc= mysql_stmt_prepare(stmt, query, strlen(query));
  check_execute(stmt, rc);

  rc= mysql_stmt_bind_param(stmt, my_bind);
  check_execute(stmt,rc);

  for (i= 0; i < 3; i++)
  {
    rc= mysql_stmt_execute(stmt);
    check_execute(stmt, rc);
    rc= my_process_stmt_result(stmt);
    DIE_UNLESS(0 == rc);
  }

  mysql_stmt_close(stmt);

  rc= mysql_query(mysql, "DROP TABLE t1");
  myquery(rc);
  rc= mysql_query(mysql, "DROP VIEW vt1");
  myquery(rc);
}