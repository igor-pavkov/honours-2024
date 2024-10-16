static void test_prepare_alter()
{
  MYSQL_STMT  *stmt;
  int         rc, id;
  MYSQL_BIND  my_bind[1];
  my_bool     is_null;

  myheader("test_prepare_alter");

  rc= mysql_query(mysql, "DROP TABLE IF EXISTS test_prep_alter");
  myquery(rc);

  rc= mysql_query(mysql, "CREATE TABLE test_prep_alter(id int, name char(20))");
  myquery(rc);

  rc= mysql_query(mysql, "INSERT INTO test_prep_alter values(10, 'venu'), (20, 'mysql')");
  myquery(rc);

  stmt= mysql_simple_prepare(mysql, "INSERT INTO test_prep_alter VALUES(?, 'monty')");
  check_stmt(stmt);

  verify_param_count(stmt, 1);

  /*
    We need to bzero bind structure because mysql_stmt_bind_param checks all
    its members.
  */
  bzero((char*) my_bind, sizeof(my_bind));

  is_null= 0;
  my_bind[0].buffer_type= MYSQL_TYPE_SHORT;
  my_bind[0].buffer= (void *)&id;
  my_bind[0].is_null= &is_null;

  rc= mysql_stmt_bind_param(stmt, my_bind);
  check_execute(stmt, rc);

  id= 30;
  rc= mysql_stmt_execute(stmt);
  check_execute(stmt, rc);

  if (thread_query("ALTER TABLE test_prep_alter change id id_new varchar(20)"))
    exit(1);

  is_null= 1;
  rc= mysql_stmt_execute(stmt);
  check_execute(stmt, rc);

  rc= my_stmt_result("SELECT * FROM test_prep_alter");
  DIE_UNLESS(rc == 4);

  mysql_stmt_close(stmt);
}