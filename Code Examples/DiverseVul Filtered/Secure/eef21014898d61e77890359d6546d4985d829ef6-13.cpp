static void test_prepare()
{
  MYSQL_STMT *stmt;
  int        rc, i;
  int        int_data, o_int_data;
  char       str_data[50], data[50];
  char       tiny_data, o_tiny_data;
  short      small_data, o_small_data;
  longlong   big_data, o_big_data;
  float      real_data, o_real_data;
  double     double_data, o_double_data;
  ulong      length[7], len;
  my_bool    is_null[7];
  char	     llbuf[22];
  MYSQL_BIND my_bind[7];
  char query[MAX_TEST_QUERY_LENGTH];

  myheader("test_prepare");

  rc= mysql_autocommit(mysql, TRUE);
  myquery(rc);

  rc= mysql_query(mysql, "DROP TABLE IF EXISTS my_prepare");
  myquery(rc);

  rc= mysql_query(mysql, "CREATE TABLE my_prepare(col1 tinyint, "
                         "col2 varchar(15), col3 int, "
                         "col4 smallint, col5 bigint, "
                         "col6 float, col7 double )");
  myquery(rc);

  /* insert by prepare */
  strxmov(query, "INSERT INTO my_prepare VALUES(?, ?, ?, ?, ?, ?, ?)", NullS);
  stmt= mysql_simple_prepare(mysql, query);
  check_stmt(stmt);

  verify_param_count(stmt, 7);

  bzero((char*) my_bind, sizeof(my_bind));

  /* tinyint */
  my_bind[0].buffer_type= MYSQL_TYPE_TINY;
  my_bind[0].buffer= (void *)&tiny_data;
  /* string */
  my_bind[1].buffer_type= MYSQL_TYPE_STRING;
  my_bind[1].buffer= (void *)str_data;
  my_bind[1].buffer_length= 1000;                  /* Max string length */
  /* integer */
  my_bind[2].buffer_type= MYSQL_TYPE_LONG;
  my_bind[2].buffer= (void *)&int_data;
  /* short */
  my_bind[3].buffer_type= MYSQL_TYPE_SHORT;
  my_bind[3].buffer= (void *)&small_data;
  /* bigint */
  my_bind[4].buffer_type= MYSQL_TYPE_LONGLONG;
  my_bind[4].buffer= (void *)&big_data;
  /* float */
  my_bind[5].buffer_type= MYSQL_TYPE_FLOAT;
  my_bind[5].buffer= (void *)&real_data;
  /* double */
  my_bind[6].buffer_type= MYSQL_TYPE_DOUBLE;
  my_bind[6].buffer= (void *)&double_data;

  for (i= 0; i < (int) array_elements(my_bind); i++)
  {
    my_bind[i].length= &length[i];
    my_bind[i].is_null= &is_null[i];
    is_null[i]= 0;
  }

  rc= mysql_stmt_bind_param(stmt, my_bind);
  check_execute(stmt, rc);

  int_data= 320;
  small_data= 1867;
  big_data= 1000;
  real_data= 2;
  double_data= 6578.001;

  /* now, execute the prepared statement to insert 10 records.. */
  for (tiny_data= 0; tiny_data < 100; tiny_data++)
  {
    length[1]= sprintf(str_data, "MySQL%d", int_data);
    rc= mysql_stmt_execute(stmt);
    check_execute(stmt, rc);
    int_data += 25;
    small_data += 10;
    big_data += 100;
    real_data += 1;
    double_data += 10.09;
  }

  mysql_stmt_close(stmt);

  /* now fetch the results ..*/
  rc= mysql_commit(mysql);
  myquery(rc);

  /* test the results now, only one row should exist */
  rc= my_stmt_result("SELECT * FROM my_prepare");
  DIE_UNLESS(tiny_data == (char) rc);

  stmt= mysql_simple_prepare(mysql, "SELECT * FROM my_prepare");
  check_stmt(stmt);

  rc= mysql_stmt_bind_result(stmt, my_bind);
  check_execute(stmt, rc);

  /* get the result */
  rc= mysql_stmt_execute(stmt);
  check_execute(stmt, rc);

  o_int_data= 320;
  o_small_data= 1867;
  o_big_data= 1000;
  o_real_data= 2;
  o_double_data= 6578.001;

  /* now, execute the prepared statement to insert 10 records.. */
  for (o_tiny_data= 0; o_tiny_data < 100; o_tiny_data++)
  {
    len= sprintf(data, "MySQL%d", o_int_data);

    rc= mysql_stmt_fetch(stmt);
    check_execute(stmt, rc);

    if (!opt_silent)
    {
      fprintf(stdout, "\n");
      fprintf(stdout, "\n\t tiny   : %d (%lu)", tiny_data, length[0]);
      fprintf(stdout, "\n\t short  : %d (%lu)", small_data, length[3]);
      fprintf(stdout, "\n\t int    : %d (%lu)", int_data, length[2]);
      fprintf(stdout, "\n\t big    : %s (%lu)", llstr(big_data, llbuf),
              length[4]);

      fprintf(stdout, "\n\t float  : %f (%lu)", real_data, length[5]);
      fprintf(stdout, "\n\t double : %f (%lu)", double_data, length[6]);

      fprintf(stdout, "\n\t str    : %s (%lu)", str_data, length[1]);
    }

    DIE_UNLESS(tiny_data == o_tiny_data);
    DIE_UNLESS(is_null[0] == 0);
    DIE_UNLESS(length[0] == 1);

    DIE_UNLESS(int_data == o_int_data);
    DIE_UNLESS(length[2] == 4);

    DIE_UNLESS(small_data == o_small_data);
    DIE_UNLESS(length[3] == 2);

    DIE_UNLESS(big_data == o_big_data);
    DIE_UNLESS(length[4] == 8);

    DIE_UNLESS(real_data == o_real_data);
    DIE_UNLESS(length[5] == 4);

    DIE_UNLESS(cmp_double(&double_data, &o_double_data));
    DIE_UNLESS(length[6] == 8);

    DIE_UNLESS(strcmp(data, str_data) == 0);
    DIE_UNLESS(length[1] == len);

    o_int_data += 25;
    o_small_data += 10;
    o_big_data += 100;
    o_real_data += 1;
    o_double_data += 10.09;
  }

  rc= mysql_stmt_fetch(stmt);
  DIE_UNLESS(rc == MYSQL_NO_DATA);

  mysql_stmt_close(stmt);

}