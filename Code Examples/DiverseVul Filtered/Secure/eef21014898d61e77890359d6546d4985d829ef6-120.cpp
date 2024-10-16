static void test_pure_coverage()
{
  MYSQL_STMT *stmt;
  MYSQL_BIND my_bind[2];
  int        rc;
  ulong      length;

  myheader("test_pure_coverage");

  rc= mysql_query(mysql, "DROP TABLE IF EXISTS test_pure");
  myquery(rc);

  rc= mysql_query(mysql, "CREATE TABLE test_pure(c1 int, c2 varchar(20))");
  myquery(rc);

  stmt= mysql_simple_prepare(mysql, "insert into test_pure(c67788) values(10)");
  check_stmt_r(stmt);

  /* Query without params and result should allow to bind 0 arrays */
  stmt= mysql_simple_prepare(mysql, "insert into test_pure(c2) values(10)");
  check_stmt(stmt);

  rc= mysql_stmt_bind_param(stmt, (MYSQL_BIND*)0);
  check_execute(stmt, rc);

  rc= mysql_stmt_execute(stmt);
  check_execute(stmt, rc);

  rc= mysql_stmt_bind_result(stmt, (MYSQL_BIND*)0);
  DIE_UNLESS(rc == 1);

  mysql_stmt_close(stmt);

  stmt= mysql_simple_prepare(mysql, "insert into test_pure(c2) values(?)");
  check_stmt(stmt);

  /*
    We need to bzero bind structure because mysql_stmt_bind_param checks all
    its members.
  */
  bzero((char*) my_bind, sizeof(my_bind));

  my_bind[0].length= &length;
  my_bind[0].is_null= 0;
  my_bind[0].buffer_length= 0;

  my_bind[0].buffer_type= MYSQL_TYPE_GEOMETRY;
  rc= mysql_stmt_bind_param(stmt, my_bind);
  check_execute_r(stmt, rc); /* unsupported buffer type */

  my_bind[0].buffer_type= MYSQL_TYPE_STRING;
  rc= mysql_stmt_bind_param(stmt, my_bind);
  check_execute(stmt, rc);

  rc= mysql_stmt_store_result(stmt);
  check_execute(stmt, rc);

  mysql_stmt_close(stmt);

  stmt= mysql_simple_prepare(mysql, "select * from test_pure");
  check_execute(stmt, rc);

  rc= mysql_stmt_execute(stmt);
  check_execute(stmt, rc);

  my_bind[0].buffer_type= MYSQL_TYPE_GEOMETRY;
  rc= mysql_stmt_bind_result(stmt, my_bind);
  check_execute_r(stmt, rc); /* unsupported buffer type */

  rc= mysql_stmt_store_result(stmt);
  DIE_UNLESS(rc);

  rc= mysql_stmt_store_result(stmt);
  DIE_UNLESS(rc); /* Old error must be reset first */

  mysql_stmt_close(stmt);

  mysql_query(mysql, "DROP TABLE test_pure");
}