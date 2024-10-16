static void test_rewind(void)
{
  MYSQL_STMT *stmt;
  MYSQL_BIND my_bind;
  int rc = 0;
  const char *stmt_text;
  long unsigned int length=4, Data=0;
  my_bool isnull=0;

  myheader("test_rewind");

  stmt_text= "CREATE TABLE t1 (a int)";
  rc= mysql_real_query(mysql, stmt_text, strlen(stmt_text));
  myquery(rc);
  stmt_text= "INSERT INTO t1 VALUES(2),(3),(4)";
  rc= mysql_real_query(mysql, stmt_text, strlen(stmt_text));
  myquery(rc);

  stmt= mysql_stmt_init(mysql);

  stmt_text= "SELECT * FROM t1";
  rc= mysql_stmt_prepare(stmt, stmt_text, strlen(stmt_text));
  check_execute(stmt, rc);

  bzero((char*) &my_bind, sizeof(MYSQL_BIND));
  my_bind.buffer_type= MYSQL_TYPE_LONG;
  my_bind.buffer= (void *)&Data; /* this buffer won't be altered */
  my_bind.length= &length;
  my_bind.is_null= &isnull;

  rc= mysql_stmt_execute(stmt);
  check_execute(stmt, rc);

  rc= mysql_stmt_store_result(stmt);
  DIE_UNLESS(rc == 0);

  rc= mysql_stmt_bind_result(stmt, &my_bind);
  DIE_UNLESS(rc == 0);

  /* retreive all result sets till we are at the end */
  while(!mysql_stmt_fetch(stmt))
    if (!opt_silent)
      printf("fetched result:%ld\n", Data);

  DIE_UNLESS(rc != MYSQL_NO_DATA);

  /* seek to the first row */
  mysql_stmt_data_seek(stmt, 0);

  /* now we should be able to fetch the results again */
  /* but mysql_stmt_fetch returns MYSQL_NO_DATA */
  while(!(rc= mysql_stmt_fetch(stmt)))
    if (!opt_silent)
      printf("fetched result after seek:%ld\n", Data);
  
  DIE_UNLESS(rc == MYSQL_NO_DATA);

  stmt_text= "DROP TABLE t1";
  rc= mysql_real_query(mysql, stmt_text, strlen(stmt_text));
  myquery(rc);
  rc= mysql_stmt_free_result(stmt);
  rc= mysql_stmt_close(stmt);
}