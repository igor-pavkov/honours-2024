static void test_buffers()
{
  MYSQL_STMT *stmt;
  MYSQL_BIND my_bind[1];
  int        rc;
  ulong      length;
  my_bool    is_null;
  char       buffer[20];

  myheader("test_buffers");

  rc= mysql_query(mysql, "DROP TABLE IF EXISTS test_buffer");
  myquery(rc);

  rc= mysql_query(mysql, "CREATE TABLE test_buffer(str varchar(20))");
  myquery(rc);

  rc= mysql_query(mysql, "insert into test_buffer values('MySQL')\
                          , ('Database'), ('Open-Source'), ('Popular')");
  myquery(rc);

  stmt= mysql_simple_prepare(mysql, "select str from test_buffer");
  check_stmt(stmt);

  rc= mysql_stmt_execute(stmt);
  check_execute(stmt, rc);

  bzero(buffer, sizeof(buffer));              /* Avoid overruns in printf() */

  bzero((char*) my_bind, sizeof(my_bind));
  my_bind[0].length= &length;
  my_bind[0].is_null= &is_null;
  my_bind[0].buffer_length= 1;
  my_bind[0].buffer_type= MYSQL_TYPE_STRING;
  my_bind[0].buffer= (void *)buffer;
  my_bind[0].error= &my_bind[0].error_value;

  rc= mysql_stmt_bind_result(stmt, my_bind);
  check_execute(stmt, rc);

  rc= mysql_stmt_store_result(stmt);
  check_execute(stmt, rc);

  buffer[1]= 'X';
  rc= mysql_stmt_fetch(stmt);
  DIE_UNLESS(rc == MYSQL_DATA_TRUNCATED);
  DIE_UNLESS(my_bind[0].error_value);
  if (!opt_silent)
    fprintf(stdout, "\n data: %s (%lu)", buffer, length);
  DIE_UNLESS(buffer[0] == 'M');
  DIE_UNLESS(buffer[1] == 'X');
  DIE_UNLESS(length == 5);

  my_bind[0].buffer_length= 8;
  rc= mysql_stmt_bind_result(stmt, my_bind);/* re-bind */
  check_execute(stmt, rc);

  rc= mysql_stmt_fetch(stmt);
  check_execute(stmt, rc);
  if (!opt_silent)
    fprintf(stdout, "\n data: %s (%lu)", buffer, length);
  DIE_UNLESS(strncmp(buffer, "Database", 8) == 0);
  DIE_UNLESS(length == 8);

  my_bind[0].buffer_length= 12;
  rc= mysql_stmt_bind_result(stmt, my_bind);/* re-bind */
  check_execute(stmt, rc);

  rc= mysql_stmt_fetch(stmt);
  check_execute(stmt, rc);
  if (!opt_silent)
    fprintf(stdout, "\n data: %s (%lu)", buffer, length);
  DIE_UNLESS(strcmp(buffer, "Open-Source") == 0);
  DIE_UNLESS(length == 11);

  my_bind[0].buffer_length= 6;
  rc= mysql_stmt_bind_result(stmt, my_bind);/* re-bind */
  check_execute(stmt, rc);

  rc= mysql_stmt_fetch(stmt);
  DIE_UNLESS(rc == MYSQL_DATA_TRUNCATED);
  DIE_UNLESS(my_bind[0].error_value);
  if (!opt_silent)
    fprintf(stdout, "\n data: %s (%lu)", buffer, length);
  DIE_UNLESS(strncmp(buffer, "Popula", 6) == 0);
  DIE_UNLESS(length == 7);

  mysql_stmt_close(stmt);
}