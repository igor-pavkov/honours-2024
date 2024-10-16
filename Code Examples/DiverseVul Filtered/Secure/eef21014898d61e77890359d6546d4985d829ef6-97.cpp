static void test_bug10729()
{
  MYSQL_STMT *stmt;
  MYSQL_BIND my_bind[1];
  char a[21];
  int rc;
  const char *stmt_text;
  int i= 0;
  const char *name_array[3]= { "aaa", "bbb", "ccc" };
  ulong type;

  myheader("test_bug10729");

  mysql_query(mysql, "drop table if exists t1");
  mysql_query(mysql, "create table t1 (id integer not null primary key,"
                                      "name VARCHAR(20) NOT NULL)");
  rc= mysql_query(mysql, "insert into t1 (id, name) values "
                         "(1, 'aaa'), (2, 'bbb'), (3, 'ccc')");
  myquery(rc);

  stmt= mysql_stmt_init(mysql);

  type= (ulong) CURSOR_TYPE_READ_ONLY;
  rc= mysql_stmt_attr_set(stmt, STMT_ATTR_CURSOR_TYPE, (void*) &type);
  check_execute(stmt, rc);
  stmt_text= "select name from t1";
  rc= mysql_stmt_prepare(stmt, stmt_text, strlen(stmt_text));
  check_execute(stmt, rc);

  bzero((char*) my_bind, sizeof(my_bind));
  my_bind[0].buffer_type= MYSQL_TYPE_STRING;
  my_bind[0].buffer= (void*) a;
  my_bind[0].buffer_length= sizeof(a);
  mysql_stmt_bind_result(stmt, my_bind);

  for (i= 0; i < 3; i++)
  {
    int row_no= 0;
    rc= mysql_stmt_execute(stmt);
    check_execute(stmt, rc);
    while ((rc= mysql_stmt_fetch(stmt)) == 0)
    {
      DIE_UNLESS(strcmp(a, name_array[row_no]) == 0);
      if (!opt_silent)
        printf("%d: %s\n", row_no, a);
      ++row_no;
    }
    DIE_UNLESS(rc == MYSQL_NO_DATA);
  }
  rc= mysql_stmt_close(stmt);
  DIE_UNLESS(rc == 0);

  rc= mysql_query(mysql, "drop table t1");
  myquery(rc);
}