static void bind_fetch(int row_count)
{
  MYSQL_STMT   *stmt;
  int          rc, i, count= row_count;
  int32        data[10];
  int8         i8_data;
  int16        i16_data;
  int32        i32_data;
  longlong     i64_data;
  float        f_data;
  double       d_data;
  char         s_data[10];
  ulong        length[10];
  MYSQL_BIND   my_bind[7];
  my_bool      is_null[7];

  stmt= mysql_simple_prepare(mysql, "INSERT INTO test_bind_fetch VALUES "
                                    "(?, ?, ?, ?, ?, ?, ?)");
  check_stmt(stmt);

  verify_param_count(stmt, 7);

  /* Always bzero all members of bind parameter */
  bzero((char*) my_bind, sizeof(my_bind));

  for (i= 0; i < (int) array_elements(my_bind); i++)
  {
    my_bind[i].buffer_type= MYSQL_TYPE_LONG;
    my_bind[i].buffer= (void *) &data[i];
  }
  rc= mysql_stmt_bind_param(stmt, my_bind);
  check_execute(stmt, rc);

  while (count--)
  {
    rc= 10+count;
    for (i= 0; i < (int) array_elements(my_bind); i++)
    {
      data[i]= rc+i;
      rc+= 12;
    }
    rc= mysql_stmt_execute(stmt);
    check_execute(stmt, rc);
  }

  rc= mysql_commit(mysql);
  myquery(rc);

  mysql_stmt_close(stmt);

  rc= my_stmt_result("SELECT * FROM test_bind_fetch");
  DIE_UNLESS(row_count == rc);

  stmt= mysql_simple_prepare(mysql, "SELECT * FROM test_bind_fetch");
  check_stmt(stmt);

  for (i= 0; i < (int) array_elements(my_bind); i++)
  {
    my_bind[i].buffer= (void *) &data[i];
    my_bind[i].length= &length[i];
    my_bind[i].is_null= &is_null[i];
  }

  my_bind[0].buffer_type= MYSQL_TYPE_TINY;
  my_bind[0].buffer= (void *)&i8_data;

  my_bind[1].buffer_type= MYSQL_TYPE_SHORT;
  my_bind[1].buffer= (void *)&i16_data;

  my_bind[2].buffer_type= MYSQL_TYPE_LONG;
  my_bind[2].buffer= (void *)&i32_data;

  my_bind[3].buffer_type= MYSQL_TYPE_LONGLONG;
  my_bind[3].buffer= (void *)&i64_data;

  my_bind[4].buffer_type= MYSQL_TYPE_FLOAT;
  my_bind[4].buffer= (void *)&f_data;

  my_bind[5].buffer_type= MYSQL_TYPE_DOUBLE;
  my_bind[5].buffer= (void *)&d_data;

  my_bind[6].buffer_type= MYSQL_TYPE_STRING;
  my_bind[6].buffer= (void *)&s_data;
  my_bind[6].buffer_length= sizeof(s_data);

  rc= mysql_stmt_bind_result(stmt, my_bind);
  check_execute(stmt, rc);

  rc= mysql_stmt_execute(stmt);
  check_execute(stmt, rc);

  rc= mysql_stmt_store_result(stmt);
  check_execute(stmt, rc);

  while (row_count--)
  {
    rc= mysql_stmt_fetch(stmt);
    check_execute(stmt, rc);

    if (!opt_silent)
    {
      fprintf(stdout, "\n");
      fprintf(stdout, "\n tiny     : %ld(%lu)", (ulong) i8_data, length[0]);
      fprintf(stdout, "\n short    : %ld(%lu)", (ulong) i16_data, length[1]);
      fprintf(stdout, "\n int      : %ld(%lu)", (ulong) i32_data, length[2]);
      fprintf(stdout, "\n longlong : %ld(%lu)", (ulong) i64_data, length[3]);
      fprintf(stdout, "\n float    : %f(%lu)",  f_data,  length[4]);
      fprintf(stdout, "\n double   : %g(%lu)",  d_data,  length[5]);
      fprintf(stdout, "\n char     : %s(%lu)",  s_data,  length[6]);
    }
    rc= 10+row_count;

    /* TINY */
    DIE_UNLESS((int) i8_data == rc);
    DIE_UNLESS(length[0] == 1);
    rc+= 13;

    /* SHORT */
    DIE_UNLESS((int) i16_data == rc);
    DIE_UNLESS(length[1] == 2);
    rc+= 13;

    /* LONG */
    DIE_UNLESS((int) i32_data == rc);
    DIE_UNLESS(length[2] == 4);
    rc+= 13;

    /* LONGLONG */
    DIE_UNLESS((int) i64_data == rc);
    DIE_UNLESS(length[3] == 8);
    rc+= 13;

    /* FLOAT */
    DIE_UNLESS((int)f_data == rc);
    DIE_UNLESS(length[4] == 4);
    rc+= 13;

    /* DOUBLE */
    DIE_UNLESS((int)d_data == rc);
    DIE_UNLESS(length[5] == 8);
    rc+= 13;

    /* CHAR */
    {
      char buff[20];
      long len= sprintf(buff, "%d", rc);
      DIE_UNLESS(strcmp(s_data, buff) == 0);
      DIE_UNLESS(length[6] == (ulong) len);
    }
  }
  rc= mysql_stmt_fetch(stmt);
  DIE_UNLESS(rc == MYSQL_NO_DATA);

  mysql_stmt_close(stmt);
}