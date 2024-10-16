static void test_truncation_option()
{
  MYSQL_STMT *stmt;
  const char *stmt_text;
  int rc;
  uint8 buf;
  my_bool option= 0;
  my_bool error;
  MYSQL_BIND my_bind;

  myheader("test_truncation_option");

  /* Prepare the test table */
  stmt_text= "select -1";

  stmt= mysql_stmt_init(mysql);
  rc= mysql_stmt_prepare(stmt, stmt_text, strlen(stmt_text));
  check_execute(stmt, rc);
  rc= mysql_stmt_execute(stmt);
  check_execute(stmt, rc);

  bzero((char*) &my_bind, sizeof(my_bind));

  my_bind.buffer= (void*) &buf;
  my_bind.buffer_type= MYSQL_TYPE_TINY;
  my_bind.is_unsigned= TRUE;
  my_bind.error= &error;

  rc= mysql_stmt_bind_result(stmt, &my_bind);
  check_execute(stmt, rc);
  rc= mysql_stmt_fetch(stmt);
  DIE_UNLESS(rc == MYSQL_DATA_TRUNCATED);
  DIE_UNLESS(error);
  rc= mysql_options(mysql, MYSQL_REPORT_DATA_TRUNCATION, (char*) &option);
  myquery(rc);
  /* need to rebind for the new setting to take effect */
  rc= mysql_stmt_bind_result(stmt, &my_bind);
  check_execute(stmt, rc);
  rc= mysql_stmt_execute(stmt);
  check_execute(stmt, rc);
  rc= mysql_stmt_fetch(stmt);
  check_execute(stmt, rc);
  /* The only change is rc - error pointers are still filled in */
  DIE_UNLESS(error == 1);
  /* restore back the defaults */
  option= 1;
  mysql_options(mysql, MYSQL_REPORT_DATA_TRUNCATION, (char*) &option);

  mysql_stmt_close(stmt);
}