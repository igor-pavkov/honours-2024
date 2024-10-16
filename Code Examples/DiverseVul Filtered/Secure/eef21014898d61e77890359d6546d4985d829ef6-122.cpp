static void test_insert()
{
  MYSQL_STMT *stmt;
  int        rc;
  char       str_data[50];
  char       tiny_data;
  MYSQL_RES  *result;
  MYSQL_BIND my_bind[2];
  ulong      length;

  myheader("test_insert");

  rc= mysql_autocommit(mysql, TRUE);
  myquery(rc);

  rc= mysql_query(mysql, "DROP TABLE IF EXISTS test_prep_insert");
  myquery(rc);

  rc= mysql_query(mysql, "CREATE TABLE test_prep_insert(col1 tinyint, \
                                col2 varchar(50))");
  myquery(rc);

  /* insert by prepare */
  stmt= mysql_simple_prepare(mysql,
                             "INSERT INTO test_prep_insert VALUES(?, ?)");
  check_stmt(stmt);

  verify_param_count(stmt, 2);

  /*
    We need to bzero bind structure because mysql_stmt_bind_param checks all
    its members.
  */
  bzero((char*) my_bind, sizeof(my_bind));

  /* tinyint */
  my_bind[0].buffer_type= MYSQL_TYPE_TINY;
  my_bind[0].buffer= (void *)&tiny_data;

  /* string */
  my_bind[1].buffer_type= MYSQL_TYPE_STRING;
  my_bind[1].buffer= str_data;
  my_bind[1].buffer_length= sizeof(str_data);;
  my_bind[1].length= &length;

  rc= mysql_stmt_bind_param(stmt, my_bind);
  check_execute(stmt, rc);

  /* now, execute the prepared statement to insert 10 records.. */
  for (tiny_data= 0; tiny_data < 3; tiny_data++)
  {
    length= sprintf(str_data, "MySQL%d", tiny_data);
    rc= mysql_stmt_execute(stmt);
    check_execute(stmt, rc);
  }

  mysql_stmt_close(stmt);

  /* now fetch the results ..*/
  rc= mysql_commit(mysql);
  myquery(rc);

  /* test the results now, only one row should exist */
  rc= mysql_query(mysql, "SELECT * FROM test_prep_insert");
  myquery(rc);

  /* get the result */
  result= mysql_store_result(mysql);
  mytest(result);

  rc= my_process_result_set(result);
  DIE_UNLESS((int) tiny_data == rc);
  mysql_free_result(result);

}