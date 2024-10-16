static void test_bind_result_ext()
{
  MYSQL_STMT *stmt;
  int        rc, i;
  uchar      t_data;
  short      s_data;
  int        i_data;
  longlong   b_data;
  float      f_data;
  double     d_data;
  char       szData[20], bData[20];
  ulong       szLength, bLength;
  MYSQL_BIND my_bind[8];
  ulong      length[8];
  my_bool    is_null[8];
  char	     llbuf[22];
  myheader("test_bind_result_ext");

  rc= mysql_query(mysql, "DROP TABLE IF EXISTS test_bind_result");
  myquery(rc);

  rc= mysql_query(mysql, "CREATE TABLE test_bind_result(c1 tinyint, "
                                                      " c2 smallint, "
                                                      " c3 int, c4 bigint, "
                                                      " c5 float, c6 double, "
                                                      " c7 varbinary(10), "
                                                      " c8 varchar(50))");
  myquery(rc);

  rc= mysql_query(mysql, "INSERT INTO test_bind_result "
                         "VALUES (19, 2999, 3999, 4999999, "
                         " 2345.6, 5678.89563, 'venu', 'mysql')");
  myquery(rc);

  rc= mysql_commit(mysql);
  myquery(rc);

  bzero((char*) my_bind, sizeof(my_bind));
  for (i= 0; i < (int) array_elements(my_bind); i++)
  {
    my_bind[i].length=  &length[i];
    my_bind[i].is_null= &is_null[i];
  }

  my_bind[0].buffer_type= MYSQL_TYPE_TINY;
  my_bind[0].buffer= (void *)&t_data;

  my_bind[1].buffer_type= MYSQL_TYPE_SHORT;
  my_bind[2].buffer_type= MYSQL_TYPE_LONG;

  my_bind[3].buffer_type= MYSQL_TYPE_LONGLONG;
  my_bind[1].buffer= (void *)&s_data;

  my_bind[2].buffer= (void *)&i_data;
  my_bind[3].buffer= (void *)&b_data;

  my_bind[4].buffer_type= MYSQL_TYPE_FLOAT;
  my_bind[4].buffer= (void *)&f_data;

  my_bind[5].buffer_type= MYSQL_TYPE_DOUBLE;
  my_bind[5].buffer= (void *)&d_data;

  my_bind[6].buffer_type= MYSQL_TYPE_STRING;
  my_bind[6].buffer= (void *)szData;
  my_bind[6].buffer_length= sizeof(szData);
  my_bind[6].length= &szLength;

  my_bind[7].buffer_type= MYSQL_TYPE_TINY_BLOB;
  my_bind[7].buffer= (void *)&bData;
  my_bind[7].length= &bLength;
  my_bind[7].buffer_length= sizeof(bData);

  stmt= mysql_simple_prepare(mysql, "select * from test_bind_result");
  check_stmt(stmt);

  rc= mysql_stmt_bind_result(stmt, my_bind);
  check_execute(stmt, rc);

  rc= mysql_stmt_execute(stmt);
  check_execute(stmt, rc);

  rc= mysql_stmt_fetch(stmt);
  check_execute(stmt, rc);

  if (!opt_silent)
  {
    fprintf(stdout, "\n data (tiny)   : %d", t_data);
    fprintf(stdout, "\n data (short)  : %d", s_data);
    fprintf(stdout, "\n data (int)    : %d", i_data);
    fprintf(stdout, "\n data (big)    : %s", llstr(b_data, llbuf));

    fprintf(stdout, "\n data (float)  : %f", f_data);
    fprintf(stdout, "\n data (double) : %f", d_data);

    fprintf(stdout, "\n data (str)    : %s(%lu)", szData, szLength);

    bData[bLength]= '\0';                         /* bData is binary */
    fprintf(stdout, "\n data (bin)    : %s(%lu)", bData, bLength);
  }

  DIE_UNLESS(t_data == 19);
  DIE_UNLESS(s_data == 2999);
  DIE_UNLESS(i_data == 3999);
  DIE_UNLESS(b_data == 4999999);
  /*DIE_UNLESS(f_data == 2345.60);*/
  /*DIE_UNLESS(d_data == 5678.89563);*/
  DIE_UNLESS(strcmp(szData, "venu") == 0);
  DIE_UNLESS(strncmp(bData, "mysql", 5) == 0);
  DIE_UNLESS(szLength == 4);
  DIE_UNLESS(bLength == 5);

  rc= mysql_stmt_fetch(stmt);
  DIE_UNLESS(rc == MYSQL_NO_DATA);

  mysql_stmt_close(stmt);
}