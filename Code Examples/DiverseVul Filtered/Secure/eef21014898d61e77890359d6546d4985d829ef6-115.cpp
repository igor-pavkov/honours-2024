static void test_bug11172()
{
  MYSQL_STMT *stmt;
  MYSQL_BIND bind_in[1], bind_out[2];
  MYSQL_TIME hired;
  int rc;
  const char *stmt_text;
  int i= 0, id;
  ulong type;

  myheader("test_bug11172");

  mysql_query(mysql, "drop table if exists t1");
  mysql_query(mysql, "create table t1 (id integer not null primary key,"
                                      "hired date not null)");
  rc= mysql_query(mysql,
                  "insert into t1 (id, hired) values (1, '1933-08-24'), "
                  "(2, '1965-01-01'), (3, '1949-08-17'), (4, '1945-07-07'), "
                  "(5, '1941-05-15'), (6, '1978-09-15'), (7, '1936-03-28')");
  myquery(rc);
  stmt= mysql_stmt_init(mysql);
  stmt_text= "SELECT id, hired FROM t1 WHERE hired=?";
  rc= mysql_stmt_prepare(stmt, stmt_text, strlen(stmt_text));
  check_execute(stmt, rc);

  type= (ulong) CURSOR_TYPE_READ_ONLY;
  mysql_stmt_attr_set(stmt, STMT_ATTR_CURSOR_TYPE, (const void*) &type);

  bzero((char*) bind_in, sizeof(bind_in));
  bzero((char*) bind_out, sizeof(bind_out));
  bzero((char*) &hired, sizeof(hired));
  hired.year= 1965;
  hired.month= 1;
  hired.day= 1;
  bind_in[0].buffer_type= MYSQL_TYPE_DATE;
  bind_in[0].buffer= (void*) &hired;
  bind_in[0].buffer_length= sizeof(hired);
  bind_out[0].buffer_type= MYSQL_TYPE_LONG;
  bind_out[0].buffer= (void*) &id;
  bind_out[1]= bind_in[0];

  for (i= 0; i < 3; i++)
  {
    rc= mysql_stmt_bind_param(stmt, bind_in);
    check_execute(stmt, rc);
    rc= mysql_stmt_bind_result(stmt, bind_out);
    check_execute(stmt, rc);
    rc= mysql_stmt_execute(stmt);
    check_execute(stmt, rc);
    while ((rc= mysql_stmt_fetch(stmt)) == 0)
    {
      if (!opt_silent)
        printf("fetched data %d:%d-%d-%d\n", id,
               hired.year, hired.month, hired.day);
    }
    DIE_UNLESS(rc == MYSQL_NO_DATA);
    if (!mysql_stmt_free_result(stmt))
      mysql_stmt_reset(stmt);
  }
  mysql_stmt_close(stmt);
  mysql_rollback(mysql);
  mysql_rollback(mysql);

  rc= mysql_query(mysql, "drop table t1");
  myquery(rc);
}