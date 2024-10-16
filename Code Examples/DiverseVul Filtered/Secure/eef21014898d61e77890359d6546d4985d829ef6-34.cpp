static void test_fetch_nobuffs()
{
  MYSQL_STMT *stmt;
  MYSQL_BIND my_bind[4];
  char       str[4][50];
  int        rc;

  myheader("test_fetch_nobuffs");

  stmt= mysql_simple_prepare(mysql, "SELECT DATABASE(), CURRENT_USER(), \
                              CURRENT_DATE(), CURRENT_TIME()");
  check_stmt(stmt);

  rc= mysql_stmt_execute(stmt);
  check_execute(stmt, rc);

  rc= 0;
  while (mysql_stmt_fetch(stmt) != MYSQL_NO_DATA)
    rc++;

  if (!opt_silent)
    fprintf(stdout, "\n total rows        : %d", rc);
  DIE_UNLESS(rc == 1);

  bzero((char*) my_bind, sizeof(MYSQL_BIND));
  my_bind[0].buffer_type= MYSQL_TYPE_STRING;
  my_bind[0].buffer= (void *)str[0];
  my_bind[0].buffer_length= sizeof(str[0]);
  my_bind[1]= my_bind[2]= my_bind[3]= my_bind[0];
  my_bind[1].buffer= (void *)str[1];
  my_bind[2].buffer= (void *)str[2];
  my_bind[3].buffer= (void *)str[3];

  rc= mysql_stmt_bind_result(stmt, my_bind);
  check_execute(stmt, rc);

  rc= mysql_stmt_execute(stmt);
  check_execute(stmt, rc);

  rc= 0;
  while (mysql_stmt_fetch(stmt) != MYSQL_NO_DATA)
  {
    rc++;
    if (!opt_silent)
    {
      fprintf(stdout, "\n CURRENT_DATABASE(): %s", str[0]);
      fprintf(stdout, "\n CURRENT_USER()    : %s", str[1]);
      fprintf(stdout, "\n CURRENT_DATE()    : %s", str[2]);
      fprintf(stdout, "\n CURRENT_TIME()    : %s", str[3]);
    }
  }
  if (!opt_silent)
    fprintf(stdout, "\n total rows        : %d", rc);
  DIE_UNLESS(rc == 1);

  mysql_stmt_close(stmt);
}