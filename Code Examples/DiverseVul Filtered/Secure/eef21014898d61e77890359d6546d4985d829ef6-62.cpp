static void test_bug10794()
{
  MYSQL_STMT *stmt, *stmt1;
  MYSQL_BIND my_bind[2];
  char a[21];
  int id_val;
  ulong a_len;
  int rc;
  const char *stmt_text;
  int i= 0;
  ulong type;

  myheader("test_bug10794");

  mysql_query(mysql, "drop table if exists t1");
  mysql_query(mysql, "create table t1 (id integer not null primary key,"
                                      "name varchar(20) not null)");
  stmt= mysql_stmt_init(mysql);
  stmt_text= "insert into t1 (id, name) values (?, ?)";
  rc= mysql_stmt_prepare(stmt, stmt_text, strlen(stmt_text));
  check_execute(stmt, rc);
  bzero((char*) my_bind, sizeof(my_bind));
  my_bind[0].buffer_type= MYSQL_TYPE_LONG;
  my_bind[0].buffer= (void*) &id_val;
  my_bind[1].buffer_type= MYSQL_TYPE_STRING;
  my_bind[1].buffer= (void*) a;
  my_bind[1].length= &a_len;
  rc= mysql_stmt_bind_param(stmt, my_bind);
  check_execute(stmt, rc);
  for (i= 0; i < 42; i++)
  {
    id_val= (i+1)*10;
    sprintf(a, "a%d", i);
    a_len= strlen(a); /* safety against broken sprintf */
    rc= mysql_stmt_execute(stmt);
    check_execute(stmt, rc);
  }
  stmt_text= "select name from t1";
  rc= mysql_stmt_prepare(stmt, stmt_text, strlen(stmt_text));
  type= (ulong) CURSOR_TYPE_READ_ONLY;
  mysql_stmt_attr_set(stmt, STMT_ATTR_CURSOR_TYPE, (const void*) &type);
  stmt1= mysql_stmt_init(mysql);
  mysql_stmt_attr_set(stmt1, STMT_ATTR_CURSOR_TYPE, (const void*) &type);
  bzero((char*) my_bind, sizeof(my_bind));
  my_bind[0].buffer_type= MYSQL_TYPE_STRING;
  my_bind[0].buffer= (void*) a;
  my_bind[0].buffer_length= sizeof(a);
  my_bind[0].length= &a_len;
  rc= mysql_stmt_bind_result(stmt, my_bind);
  check_execute(stmt, rc);
  rc= mysql_stmt_execute(stmt);
  check_execute(stmt, rc);
  rc= mysql_stmt_fetch(stmt);
  check_execute(stmt, rc);
  if (!opt_silent)
    printf("Fetched row from stmt: %s\n", a);
  /* Don't optimize: an attribute of the original test case */
  mysql_stmt_free_result(stmt);
  mysql_stmt_reset(stmt);
  stmt_text= "select name from t1 where id=10";
  rc= mysql_stmt_prepare(stmt1, stmt_text, strlen(stmt_text));
  check_execute(stmt1, rc);
  rc= mysql_stmt_bind_result(stmt1, my_bind);
  check_execute(stmt1, rc);
  rc= mysql_stmt_execute(stmt1);
  while (1)
  {
    rc= mysql_stmt_fetch(stmt1);
    if (rc == MYSQL_NO_DATA)
    {
      if (!opt_silent)
        printf("End of data in stmt1\n");
      break;
    }
    check_execute(stmt1, rc);
    if (!opt_silent)
      printf("Fetched row from stmt1: %s\n", a);
  }
  mysql_stmt_close(stmt);
  mysql_stmt_close(stmt1);

  rc= mysql_query(mysql, "drop table t1");
  myquery(rc);
}