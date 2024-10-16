static void test_sshort_bug()
{
  MYSQL_STMT *stmt;
  MYSQL_BIND my_bind[4];
  short      short_value;
  int32      long_value;
  ulong      s_length, l_length, ll_length, t_length;
  ulonglong  longlong_value;
  int        rc;
  uchar      tiny_value;
  char       llbuf[22];

  myheader("test_sshort_bug");

  rc= mysql_query(mysql, "DROP TABLE IF EXISTS test_sshort");
  myquery(rc);

  rc= mysql_query(mysql, "CREATE TABLE test_sshort(a smallint signed, \
                                                  b smallint signed, \
                                                  c smallint unsigned, \
                                                  d smallint unsigned)");
  myquery(rc);

  rc= mysql_query(mysql, "INSERT INTO test_sshort VALUES(-5999, -5999, 35999, 200)");
  myquery(rc);


  stmt= mysql_simple_prepare(mysql, "SELECT * FROM test_sshort");
  check_stmt(stmt);

  rc= mysql_stmt_execute(stmt);
  check_execute(stmt, rc);

  bzero((char*) my_bind, sizeof(my_bind));
  my_bind[0].buffer_type= MYSQL_TYPE_SHORT;
  my_bind[0].buffer= (void *)&short_value;
  my_bind[0].length= &s_length;

  my_bind[1].buffer_type= MYSQL_TYPE_LONG;
  my_bind[1].buffer= (void *)&long_value;
  my_bind[1].length= &l_length;

  my_bind[2].buffer_type= MYSQL_TYPE_LONGLONG;
  my_bind[2].buffer= (void *)&longlong_value;
  my_bind[2].length= &ll_length;

  my_bind[3].buffer_type= MYSQL_TYPE_TINY;
  my_bind[3].buffer= (void *)&tiny_value;
  my_bind[3].is_unsigned= TRUE;
  my_bind[3].length= &t_length;

  rc= mysql_stmt_bind_result(stmt, my_bind);
  check_execute(stmt, rc);

  rc= mysql_stmt_fetch(stmt);
  check_execute(stmt, rc);

  if (!opt_silent)
  {
    fprintf(stdout, "\n sshort   : %d (%ld)", short_value, s_length);
    fprintf(stdout, "\n slong    : %ld (%ld)", (long) long_value, l_length);
    fprintf(stdout, "\n longlong : %s (%ld)", llstr(longlong_value, llbuf),
            ll_length);
    fprintf(stdout, "\n tinyint  : %d   (%ld)", tiny_value, t_length);
  }

  DIE_UNLESS(short_value == -5999);
  DIE_UNLESS(s_length == 2);

  DIE_UNLESS(long_value == -5999);
  DIE_UNLESS(l_length == 4);

  DIE_UNLESS(longlong_value == 35999);
  DIE_UNLESS(ll_length == 8);

  DIE_UNLESS(tiny_value == 200);
  DIE_UNLESS(t_length == 1);

  rc= mysql_stmt_fetch(stmt);
  DIE_UNLESS(rc == MYSQL_NO_DATA);

  mysql_stmt_close(stmt);
}