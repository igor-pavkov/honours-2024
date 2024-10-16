static void test_bug10760()
{
  MYSQL_STMT *stmt;
  MYSQL_BIND my_bind[1];
  int rc;
  const char *stmt_text;
  char id_buf[20];
  ulong id_len;
  int i= 0;
  ulong type;

  myheader("test_bug10760");

  mysql_query(mysql, "drop table if exists t1, t2");

  /* create tables */
  rc= mysql_query(mysql, "create table t1 (id integer not null primary key)"
                         " engine=MyISAM");
  myquery(rc);
  for (; i < 42; ++i)
  {
    char buf[100];
    sprintf(buf, "insert into t1 (id) values (%d)", i+1);
    rc= mysql_query(mysql, buf);
    myquery(rc);
  }
  mysql_autocommit(mysql, FALSE);
  /* create statement */
  stmt= mysql_stmt_init(mysql);
  type= (ulong) CURSOR_TYPE_READ_ONLY;
  mysql_stmt_attr_set(stmt, STMT_ATTR_CURSOR_TYPE, (const void*) &type);

  /*
    1: check that a deadlock within the same connection
    is resolved and an error is returned. The deadlock is modelled
    as follows:
    con1: open cursor for select * from t1;
    con1: insert into t1 (id) values (1)
  */
  stmt_text= "select id from t1 order by 1";
  rc= mysql_stmt_prepare(stmt, stmt_text, strlen(stmt_text));
  check_execute(stmt, rc);
  rc= mysql_stmt_execute(stmt);
  check_execute(stmt, rc);
  rc= mysql_query(mysql, "update t1 set id=id+100");
  /*
    If cursors are not materialized, the update will return an error;
    we mainly test that it won't deadlock.
  */
  if (rc && !opt_silent)
    printf("Got error (as expected): %s\n", mysql_error(mysql));
  /*
    2: check that MyISAM tables used in cursors survive
    COMMIT/ROLLBACK.
  */
  rc= mysql_rollback(mysql);                  /* should not close the cursor */
  myquery(rc);
  rc= mysql_stmt_fetch(stmt);
  check_execute(stmt, rc);

  /*
    3: check that cursors to InnoDB tables are closed (for now) by
    COMMIT/ROLLBACK.
  */
  if (! have_innodb)
  {
    if (!opt_silent)
      printf("Testing that cursors are closed at COMMIT/ROLLBACK requires "
             "InnoDB.\n");
  }
  else
  {
    stmt_text= "select id from t1 order by 1";
    rc= mysql_stmt_prepare(stmt, stmt_text, strlen(stmt_text));
    check_execute(stmt, rc);

    rc= mysql_query(mysql, "alter table t1 engine=InnoDB");
    myquery(rc);

    bzero(my_bind, sizeof(my_bind));
    my_bind[0].buffer_type= MYSQL_TYPE_STRING;
    my_bind[0].buffer= (void*) id_buf;
    my_bind[0].buffer_length= sizeof(id_buf);
    my_bind[0].length= &id_len;
    check_execute(stmt, rc);
    mysql_stmt_bind_result(stmt, my_bind);

    rc= mysql_stmt_execute(stmt);
    rc= mysql_stmt_fetch(stmt);
    DIE_UNLESS(rc == 0);
    if (!opt_silent)
      printf("Fetched row %s\n", id_buf);
    rc= mysql_rollback(mysql);                  /* should close the cursor */
    myquery(rc);
#if 0
    rc= mysql_stmt_fetch(stmt);
    DIE_UNLESS(rc);
    if (!opt_silent)
      printf("Got error (as expected): %s\n", mysql_error(mysql));
#endif
  }

  mysql_stmt_close(stmt);
  rc= mysql_query(mysql, "drop table t1");
  myquery(rc);
  mysql_autocommit(mysql, TRUE);                /* restore default */
}