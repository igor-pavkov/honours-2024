static void test_multi_stmt()
{

  MYSQL_STMT  *stmt, *stmt1, *stmt2;
  int         rc;
  uint32      id;
  char        name[50];
  MYSQL_BIND  my_bind[2];
  ulong       length[2];
  my_bool     is_null[2];
  myheader("test_multi_stmt");

  rc= mysql_query(mysql, "DROP TABLE IF EXISTS test_multi_table");
  myquery(rc);

  rc= mysql_query(mysql, "CREATE TABLE test_multi_table(id int, name char(20))");
  myquery(rc);

  rc= mysql_query(mysql, "INSERT INTO test_multi_table values(10, 'mysql')");
  myquery(rc);

  stmt= mysql_simple_prepare(mysql, "SELECT * FROM test_multi_table "
                                    "WHERE id= ?");
  check_stmt(stmt);

  stmt2= mysql_simple_prepare(mysql, "UPDATE test_multi_table "
                                     "SET name='updated' WHERE id=10");
  check_stmt(stmt2);

  verify_param_count(stmt, 1);

  /*
    We need to bzero bind structure because mysql_stmt_bind_param checks all
    its members.
  */
  bzero((char*) my_bind, sizeof(my_bind));

  my_bind[0].buffer_type= MYSQL_TYPE_LONG;
  my_bind[0].buffer= (void *)&id;
  my_bind[0].is_null= &is_null[0];
  my_bind[0].length= &length[0];
  is_null[0]= 0;
  length[0]= 0;

  my_bind[1].buffer_type= MYSQL_TYPE_STRING;
  my_bind[1].buffer= (void *)name;
  my_bind[1].buffer_length= sizeof(name);
  my_bind[1].length= &length[1];
  my_bind[1].is_null= &is_null[1];

  rc= mysql_stmt_bind_param(stmt, my_bind);
  check_execute(stmt, rc);

  rc= mysql_stmt_bind_result(stmt, my_bind);
  check_execute(stmt, rc);

  id= 10;
  rc= mysql_stmt_execute(stmt);
  check_execute(stmt, rc);

  id= 999;
  rc= mysql_stmt_fetch(stmt);
  check_execute(stmt, rc);

  if (!opt_silent)
  {
    fprintf(stdout, "\n int_data: %lu(%lu)", (ulong) id, length[0]);
    fprintf(stdout, "\n str_data: %s(%lu)", name, length[1]);
  }
  DIE_UNLESS(id == 10);
  DIE_UNLESS(strcmp(name, "mysql") == 0);

  rc= mysql_stmt_fetch(stmt);
  DIE_UNLESS(rc == MYSQL_NO_DATA);

  /* alter the table schema now */
  stmt1= mysql_simple_prepare(mysql, "DELETE FROM test_multi_table "
                                     "WHERE id= ? AND "
                                     "CONVERT(name USING utf8)=?");
  check_stmt(stmt1);

  verify_param_count(stmt1, 2);

  rc= mysql_stmt_bind_param(stmt1, my_bind);
  check_execute(stmt1, rc);

  rc= mysql_stmt_execute(stmt2);
  check_execute(stmt2, rc);

  verify_st_affected_rows(stmt2, 1);

  rc= mysql_stmt_execute(stmt);
  check_execute(stmt, rc);

  rc= mysql_stmt_fetch(stmt);
  check_execute(stmt, rc);

  if (!opt_silent)
  {
    fprintf(stdout, "\n int_data: %lu(%lu)", (ulong) id, length[0]);
    fprintf(stdout, "\n str_data: %s(%lu)", name, length[1]);
  }
  DIE_UNLESS(id == 10);
  DIE_UNLESS(strcmp(name, "updated") == 0);

  rc= mysql_stmt_fetch(stmt);
  DIE_UNLESS(rc == MYSQL_NO_DATA);

  rc= mysql_stmt_execute(stmt1);
  check_execute(stmt1, rc);

  verify_st_affected_rows(stmt1, 1);

  mysql_stmt_close(stmt1);

  rc= mysql_stmt_execute(stmt);
  check_execute(stmt, rc);

  rc= mysql_stmt_fetch(stmt);
  DIE_UNLESS(rc == MYSQL_NO_DATA);

  rc= my_stmt_result("SELECT * FROM test_multi_table");
  DIE_UNLESS(rc == 0);

  mysql_stmt_close(stmt);
  mysql_stmt_close(stmt2);

}