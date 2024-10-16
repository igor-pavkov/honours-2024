static void test_bug9478()
{
  MYSQL_STMT *stmt;
  MYSQL_BIND my_bind[1];
  char a[6];
  ulong a_len;
  int rc, i;
  DBUG_ENTER("test_bug9478");

  myheader("test_bug9478");

  mysql_query(mysql, "drop table if exists t1");
  mysql_query(mysql, "create table t1 (id integer not null primary key, "
                     " name varchar(20) not null)");
  rc= mysql_query(mysql, "insert into t1 (id, name) values "
                         " (1, 'aaa'), (2, 'bbb'), (3, 'ccc')");
  myquery(rc);

  stmt= open_cursor("select name from t1 where id=2");

  bzero((char*) my_bind, sizeof(my_bind));
  my_bind[0].buffer_type= MYSQL_TYPE_STRING;
  my_bind[0].buffer= (char*) a;
  my_bind[0].buffer_length= sizeof(a);
  my_bind[0].length= &a_len;
  mysql_stmt_bind_result(stmt, my_bind);

  for (i= 0; i < 5; i++)
  {
    rc= mysql_stmt_execute(stmt);
    check_execute(stmt, rc);
    rc= mysql_stmt_fetch(stmt);
    check_execute(stmt, rc);
    if (!opt_silent && i == 0)
      printf("Fetched row: %s\n", a);

    /*
      The query above is a one-row result set. Therefore, there is no
      cursor associated with it, as the server won't bother with opening
      a cursor for a one-row result set. The first row was read from the
      server in the fetch above. But there is eof packet pending in the
      network. mysql_stmt_execute will flush the packet and successfully
      execute the statement.
    */

    rc= mysql_stmt_execute(stmt);
    check_execute(stmt, rc);

    rc= mysql_stmt_fetch(stmt);
    check_execute(stmt, rc);
    if (!opt_silent && i == 0)
      printf("Fetched row: %s\n", a);
    rc= mysql_stmt_fetch(stmt);
    DIE_UNLESS(rc == MYSQL_NO_DATA);

    {
      char buff[8];
      /* Fill in the fetch packet */
      int4store(buff, stmt->stmt_id);
      buff[4]= 1;                               /* prefetch rows */
      rc= ((*mysql->methods->advanced_command)(mysql, COM_STMT_FETCH,
                                               (uchar*) buff,
                                               sizeof(buff), 0,0,1,NULL) ||
           (*mysql->methods->read_query_result)(mysql));
      DIE_UNLESS(rc);
      if (!opt_silent && i == 0)
        printf("Got error (as expected): %s\n", mysql_error(mysql));
    }

    rc= mysql_stmt_execute(stmt);
    check_execute(stmt, rc);

    rc= mysql_stmt_fetch(stmt);
    check_execute(stmt, rc);
    if (!opt_silent && i == 0)
      printf("Fetched row: %s\n", a);

    rc= mysql_stmt_reset(stmt);
    check_execute(stmt, rc);
    rc= mysql_stmt_fetch(stmt);
    DIE_UNLESS(rc && mysql_stmt_errno(stmt));
    if (!opt_silent && i == 0)
      printf("Got error (as expected): %s\n", mysql_stmt_error(stmt));
  }
  rc= mysql_stmt_close(stmt);
  DIE_UNLESS(rc == 0);

  /* Test the case with a server side cursor */
  stmt= open_cursor("select name from t1");

  mysql_stmt_bind_result(stmt, my_bind);

  for (i= 0; i < 5; i++)
  {
    DBUG_PRINT("loop",("i: %d", i));
    rc= mysql_stmt_execute(stmt);
    check_execute(stmt, rc);
    rc= mysql_stmt_fetch(stmt);
    check_execute(stmt, rc);
    if (!opt_silent && i == 0)
      printf("Fetched row: %s\n", a);
    rc= mysql_stmt_execute(stmt);
    check_execute(stmt, rc);

    while (! (rc= mysql_stmt_fetch(stmt)))
    {
      if (!opt_silent && i == 0)
        printf("Fetched row: %s\n", a);
    }
    DIE_UNLESS(rc == MYSQL_NO_DATA);

    rc= mysql_stmt_execute(stmt);
    check_execute(stmt, rc);

    rc= mysql_stmt_fetch(stmt);
    check_execute(stmt, rc);
    if (!opt_silent && i == 0)
      printf("Fetched row: %s\n", a);

    rc= mysql_stmt_reset(stmt);
    check_execute(stmt, rc);
    rc= mysql_stmt_fetch(stmt);
    DIE_UNLESS(rc && mysql_stmt_errno(stmt));
    if (!opt_silent && i == 0)
      printf("Got error (as expected): %s\n", mysql_stmt_error(stmt));
  }

  rc= mysql_stmt_close(stmt);
  DIE_UNLESS(rc == 0);

  rc= mysql_query(mysql, "drop table t1");
  myquery(rc);
  DBUG_VOID_RETURN;
}