static void test_wl4166_1()
{
  MYSQL_STMT *stmt;
  int        int_data;
  char       str_data[50];
  char       tiny_data;
  short      small_data;
  longlong   big_data;
  float      real_data;
  double     double_data;
  ulong      length[7];
  my_bool    is_null[7];
  MYSQL_BIND my_bind[7];
  int rc;
  int i;

  myheader("test_wl4166_1");

  rc= mysql_query(mysql, "DROP TABLE IF EXISTS table_4166");
  myquery(rc);

  rc= mysql_query(mysql, "CREATE TABLE table_4166(col1 tinyint NOT NULL, "
                         "col2 varchar(15), col3 int, "
                         "col4 smallint, col5 bigint, "
                         "col6 float, col7 double, "
                         "colX varchar(10) default NULL)");
  myquery(rc);

  stmt= mysql_simple_prepare(mysql,
    "INSERT INTO table_4166(col1, col2, col3, col4, col5, col6, col7) "
    "VALUES(?, ?, ?, ?, ?, ?, ?)");
  check_stmt(stmt);

  verify_param_count(stmt, 7);

  bzero(my_bind, sizeof(my_bind));
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
  for (tiny_data= 0; tiny_data < 10; tiny_data++)
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

  /* force a re-prepare with some DDL */

  rc= mysql_query(mysql,
    "ALTER TABLE table_4166 change colX colX varchar(20) default NULL");
  myquery(rc);

  /*
    execute the prepared statement again,
    without changing the types of parameters already bound.
  */

  for (tiny_data= 50; tiny_data < 60; tiny_data++)
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

  rc= mysql_query(mysql, "DROP TABLE table_4166");
  myquery(rc);
}