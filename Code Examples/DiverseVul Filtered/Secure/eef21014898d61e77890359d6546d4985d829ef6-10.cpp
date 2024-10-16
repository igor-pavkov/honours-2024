static void test_bind_result_ext1()
{
  MYSQL_STMT *stmt;
  uint       i;
  int        rc;
  char       t_data[20];
  float      s_data;
  short      i_data;
  uchar      b_data;
  int        f_data;
  long       bData;
  char       d_data[20];
  double     szData;
  MYSQL_BIND my_bind[8];
  ulong      length[8];
  my_bool    is_null[8];
  myheader("test_bind_result_ext1");

  rc= mysql_query(mysql, "DROP TABLE IF EXISTS test_bind_result");
  myquery(rc);

  rc= mysql_query(mysql, "CREATE TABLE test_bind_result(c1 tinyint, c2 smallint, \
                                                        c3 int, c4 bigint, \
                                                        c5 float, c6 double, \
                                                        c7 varbinary(10), \
                                                        c8 varchar(10))");
  myquery(rc);

  rc= mysql_query(mysql, "INSERT INTO test_bind_result VALUES(120, 2999, 3999, 54, \
                                                              2.6, 58.89, \
                                                              '206', '6.7')");
  myquery(rc);

  rc= mysql_commit(mysql);
  myquery(rc);

  bzero((char*) my_bind, sizeof(my_bind));
  my_bind[0].buffer_type= MYSQL_TYPE_STRING;
  my_bind[0].buffer= (void *) t_data;
  my_bind[0].buffer_length= sizeof(t_data);
  my_bind[0].error= &my_bind[0].error_value;

  my_bind[1].buffer_type= MYSQL_TYPE_FLOAT;
  my_bind[1].buffer= (void *)&s_data;
  my_bind[1].buffer_length= 0;
  my_bind[1].error= &my_bind[1].error_value;

  my_bind[2].buffer_type= MYSQL_TYPE_SHORT;
  my_bind[2].buffer= (void *)&i_data;
  my_bind[2].buffer_length= 0;
  my_bind[2].error= &my_bind[2].error_value;

  my_bind[3].buffer_type= MYSQL_TYPE_TINY;
  my_bind[3].buffer= (void *)&b_data;
  my_bind[3].buffer_length= 0;
  my_bind[3].error= &my_bind[3].error_value;

  my_bind[4].buffer_type= MYSQL_TYPE_LONG;
  my_bind[4].buffer= (void *)&f_data;
  my_bind[4].buffer_length= 0;
  my_bind[4].error= &my_bind[4].error_value;

  my_bind[5].buffer_type= MYSQL_TYPE_STRING;
  my_bind[5].buffer= (void *)d_data;
  my_bind[5].buffer_length= sizeof(d_data);
  my_bind[5].error= &my_bind[5].error_value;

  my_bind[6].buffer_type= MYSQL_TYPE_LONG;
  my_bind[6].buffer= (void *)&bData;
  my_bind[6].buffer_length= 0;
  my_bind[6].error= &my_bind[6].error_value;

  my_bind[7].buffer_type= MYSQL_TYPE_DOUBLE;
  my_bind[7].buffer= (void *)&szData;
  my_bind[7].buffer_length= 0;
  my_bind[7].error= &my_bind[7].error_value;

  for (i= 0; i < array_elements(my_bind); i++)
  {
    my_bind[i].is_null= &is_null[i];
    my_bind[i].length= &length[i];
  }

  stmt= mysql_simple_prepare(mysql, "select * from test_bind_result");
  check_stmt(stmt);

  rc= mysql_stmt_bind_result(stmt, my_bind);
  check_execute(stmt, rc);

  rc= mysql_stmt_execute(stmt);
  check_execute(stmt, rc);

  rc= mysql_stmt_fetch(stmt);
  printf("rc=%d\n", rc);
  DIE_UNLESS(rc == 0);

  if (!opt_silent)
  {
    fprintf(stdout, "\n data (tiny)   : %s(%lu)", t_data, length[0]);
    fprintf(stdout, "\n data (short)  : %f(%lu)", s_data, length[1]);
    fprintf(stdout, "\n data (int)    : %d(%lu)", i_data, length[2]);
    fprintf(stdout, "\n data (big)    : %d(%lu)", b_data, length[3]);

    fprintf(stdout, "\n data (float)  : %d(%lu)", f_data, length[4]);
    fprintf(stdout, "\n data (double) : %s(%lu)", d_data, length[5]);

    fprintf(stdout, "\n data (bin)    : %ld(%lu)", bData, length[6]);
    fprintf(stdout, "\n data (str)    : %g(%lu)", szData, length[7]);
  }

  DIE_UNLESS(strcmp(t_data, "120") == 0);
  DIE_UNLESS(i_data == 3999);
  DIE_UNLESS(f_data == 2);
  DIE_UNLESS(strcmp(d_data, "58.89") == 0);
  DIE_UNLESS(b_data == 54);

  DIE_UNLESS(length[0] == 3);
  DIE_UNLESS(length[1] == 4);
  DIE_UNLESS(length[2] == 2);
  DIE_UNLESS(length[3] == 1);
  DIE_UNLESS(length[4] == 4);
  DIE_UNLESS(length[5] == 5);
  DIE_UNLESS(length[6] == 4);
  DIE_UNLESS(length[7] == 8);

  rc= mysql_stmt_fetch(stmt);
  DIE_UNLESS(rc == MYSQL_NO_DATA);

  mysql_stmt_close(stmt);
}