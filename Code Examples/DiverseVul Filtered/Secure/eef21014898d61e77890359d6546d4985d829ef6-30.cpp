static void test_derived()
{
  MYSQL_STMT *stmt;
  int rc, i;
  MYSQL_BIND      my_bind[1];
  int32           my_val= 0;
  ulong           my_length= 0L;
  my_bool         my_null= FALSE;
  const char *query=
    "select count(1) from (select f.id from t1 f where f.id=?) as x";

  myheader("test_derived");

  rc= mysql_query(mysql, "DROP TABLE IF EXISTS t1");
  myquery(rc);

  rc= mysql_query(mysql, "create table t1 (id  int(8), primary key (id)) \
ENGINE=InnoDB DEFAULT CHARSET=utf8");
  myquery(rc);

  rc= mysql_query(mysql, "insert into t1 values (1)");
  myquery(rc);

  stmt= mysql_simple_prepare(mysql, query);
  check_stmt(stmt);
  /*
    We need to bzero bind structure because mysql_stmt_bind_param checks all
    its members.
  */
  bzero((char*) my_bind, sizeof(my_bind));

  my_bind[0].buffer_type= MYSQL_TYPE_LONG;
  my_bind[0].buffer= (void *)&my_val;
  my_bind[0].length= &my_length;
  my_bind[0].is_null= (char*)&my_null;
  my_val= 1;
  rc= mysql_stmt_bind_param(stmt, my_bind);
  check_execute(stmt, rc);

  for (i= 0; i < 3; i++)
  {
    rc= mysql_stmt_execute(stmt);
    check_execute(stmt, rc);
    rc= my_process_stmt_result(stmt);
    DIE_UNLESS(rc == 1);
  }
  mysql_stmt_close(stmt);

  rc= mysql_query(mysql, "DROP TABLE t1");
  myquery(rc);
}