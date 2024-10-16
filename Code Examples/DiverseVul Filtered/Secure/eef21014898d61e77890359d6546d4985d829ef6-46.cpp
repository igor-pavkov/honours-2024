static void test_bug8330()
{
  const char *stmt_text;
  MYSQL_STMT *stmt[2];
  int i, rc;
  const char *query= "select a,b from t1 where a=?";
  MYSQL_BIND my_bind[2];
  long lval[2];

  myheader("test_bug8330");

  stmt_text= "drop table if exists t1";
  /* in case some previos test failed */
  rc= mysql_real_query(mysql, stmt_text, strlen(stmt_text));
  myquery(rc);
  stmt_text= "create table t1 (a int, b int)";
  rc= mysql_real_query(mysql, stmt_text, strlen(stmt_text));
  myquery(rc);

  bzero((char*) my_bind, sizeof(my_bind));
  for (i=0; i < 2; i++)
  {
    stmt[i]= mysql_stmt_init(mysql);
    rc= mysql_stmt_prepare(stmt[i], query, strlen(query));
    check_execute(stmt[i], rc);

    my_bind[i].buffer_type= MYSQL_TYPE_LONG;
    my_bind[i].buffer= (void*) &lval[i];
    my_bind[i].is_null= 0;
    mysql_stmt_bind_param(stmt[i], &my_bind[i]);
  }

  rc= mysql_stmt_execute(stmt[0]);
  check_execute(stmt[0], rc);

  rc= mysql_stmt_execute(stmt[1]);
  DIE_UNLESS(rc && mysql_stmt_errno(stmt[1]) == CR_COMMANDS_OUT_OF_SYNC);
  rc= mysql_stmt_execute(stmt[0]);
  check_execute(stmt[0], rc);

  mysql_stmt_close(stmt[0]);
  mysql_stmt_close(stmt[1]);

  stmt_text= "drop table t1";
  rc= mysql_real_query(mysql, stmt_text, strlen(stmt_text));
  myquery(rc);
}