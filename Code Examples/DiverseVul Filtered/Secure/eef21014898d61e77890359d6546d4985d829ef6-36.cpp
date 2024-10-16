static void test_bug9520()
{
  MYSQL_STMT *stmt;
  MYSQL_BIND my_bind[1];
  char a[6];
  ulong a_len;
  int rc, row_count= 0;

  myheader("test_bug9520");

  mysql_query(mysql, "drop table if exists t1");
  mysql_query(mysql, "create table t1 (a char(5), b char(5), c char(5),"
                     " primary key (a, b, c))");
  rc= mysql_query(mysql, "insert into t1 values ('x', 'y', 'z'), "
                  " ('a', 'b', 'c'), ('k', 'l', 'm')");
  myquery(rc);

  stmt= open_cursor("select distinct b from t1");

  /*
    Not crashes with:
    stmt= open_cursor("select distinct a from t1");
  */

  rc= mysql_stmt_execute(stmt);
  check_execute(stmt, rc);

  bzero((char*) my_bind, sizeof(my_bind));
  my_bind[0].buffer_type= MYSQL_TYPE_STRING;
  my_bind[0].buffer= (char*) a;
  my_bind[0].buffer_length= sizeof(a);
  my_bind[0].length= &a_len;

  mysql_stmt_bind_result(stmt, my_bind);

  while (!(rc= mysql_stmt_fetch(stmt)))
    row_count++;

  DIE_UNLESS(rc == MYSQL_NO_DATA);

  if (!opt_silent)
    printf("Fetched %d rows\n", row_count);
  DBUG_ASSERT(row_count == 3);

  mysql_stmt_close(stmt);

  rc= mysql_query(mysql, "drop table t1");
  myquery(rc);
}