static void test_view_insert()
{
  MYSQL_STMT *insert_stmt, *select_stmt;
  int rc, i;
  MYSQL_BIND      my_bind[1];
  int             my_val = 0;
  ulong           my_length = 0L;
  long            my_null = 0L;
  const char *query=
    "insert into v1 values (?)";

  myheader("test_view_insert");

  rc = mysql_query(mysql, "DROP TABLE IF EXISTS t1,v1");
  myquery(rc);
  rc = mysql_query(mysql, "DROP VIEW IF EXISTS t1,v1");
  myquery(rc);

  rc= mysql_query(mysql,"create table t1 (a int, primary key (a))");
  myquery(rc);

  rc= mysql_query(mysql, "create view v1 as select a from t1 where a>=1");
  myquery(rc);

  insert_stmt= mysql_stmt_init(mysql);
  rc= mysql_stmt_prepare(insert_stmt, query, strlen(query));
  check_execute(insert_stmt, rc);
  query= "select * from t1";
  select_stmt= mysql_stmt_init(mysql);
  rc= mysql_stmt_prepare(select_stmt, query, strlen(query));
  check_execute(select_stmt, rc);

  bzero((char*) my_bind, sizeof(my_bind));
  my_bind[0].buffer_type = MYSQL_TYPE_LONG;
  my_bind[0].buffer = (char *)&my_val;
  my_bind[0].length = &my_length;
  my_bind[0].is_null = (char*)&my_null;
  rc= mysql_stmt_bind_param(insert_stmt, my_bind);
  check_execute(insert_stmt, rc);

  for (i= 0; i < 3; i++)
  {
    int rowcount= 0;
    my_val= i;

    rc= mysql_stmt_execute(insert_stmt);
    check_execute(insert_stmt, rc);

    rc= mysql_stmt_execute(select_stmt);
    check_execute(select_stmt, rc);
    rowcount= (int)my_process_stmt_result(select_stmt);
    DIE_UNLESS((i+1) == rowcount);
  }
  mysql_stmt_close(insert_stmt);
  mysql_stmt_close(select_stmt);

  rc= mysql_query(mysql, "DROP VIEW v1");
  myquery(rc);
  rc= mysql_query(mysql, "DROP TABLE t1");
  myquery(rc);
}