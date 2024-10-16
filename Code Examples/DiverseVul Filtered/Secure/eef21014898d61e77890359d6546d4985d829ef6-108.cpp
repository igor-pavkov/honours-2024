static void test_subselect()
{

  MYSQL_STMT *stmt;
  int        rc, id;
  MYSQL_BIND my_bind[1];
  DBUG_ENTER("test_subselect");

  myheader("test_subselect");

  rc= mysql_query(mysql, "DROP TABLE IF EXISTS test_sub1");
  myquery(rc);

  rc= mysql_query(mysql, "DROP TABLE IF EXISTS test_sub2");
  myquery(rc);

  rc= mysql_query(mysql, "CREATE TABLE test_sub1(id int)");
  myquery(rc);

  rc= mysql_query(mysql, "CREATE TABLE test_sub2(id int, id1 int)");
  myquery(rc);

  rc= mysql_query(mysql, "INSERT INTO test_sub1 values(2)");
  myquery(rc);

  rc= mysql_query(mysql, "INSERT INTO test_sub2 VALUES(1, 7), (2, 7)");
  myquery(rc);

  rc= mysql_commit(mysql);
  myquery(rc);

  /* fetch */
  /*
    We need to bzero bind structure because mysql_stmt_bind_param checks all
    its members.
  */
  bzero((char*) my_bind, sizeof(my_bind));

  my_bind[0].buffer_type= MYSQL_TYPE_LONG;
  my_bind[0].buffer= (void *) &id;
  my_bind[0].length= 0;
  my_bind[0].is_null= 0;

  stmt= mysql_simple_prepare(mysql, "INSERT INTO test_sub2(id) SELECT * FROM test_sub1 WHERE id= ?");
  check_stmt(stmt);

  rc= mysql_stmt_bind_param(stmt, my_bind);
  check_execute(stmt, rc);

  id= 2;
  rc= mysql_stmt_execute(stmt);
  check_execute(stmt, rc);

  verify_st_affected_rows(stmt, 1);

  id= 9;
  rc= mysql_stmt_execute(stmt);
  check_execute(stmt, rc);

  verify_st_affected_rows(stmt, 0);

  mysql_stmt_close(stmt);

  rc= my_stmt_result("SELECT * FROM test_sub2");
  DIE_UNLESS(rc == 3);

  rc= my_stmt_result("SELECT ROW(1, 7) IN (select id, id1 "
                     "from test_sub2 WHERE id1= 8)");
  DIE_UNLESS(rc == 1);
  rc= my_stmt_result("SELECT ROW(1, 7) IN (select id, id1 "
                     "from test_sub2 WHERE id1= 7)");
  DIE_UNLESS(rc == 1);

  stmt= mysql_simple_prepare(mysql, ("SELECT ROW(1, 7) IN (select id, id1 "
                                     "from test_sub2 WHERE id1= ?)"));
  check_stmt(stmt);

  rc= mysql_stmt_bind_param(stmt, my_bind);
  check_execute(stmt, rc);

  rc= mysql_stmt_bind_result(stmt, my_bind);
  check_execute(stmt, rc);

  id= 7;
  rc= mysql_stmt_execute(stmt);
  check_execute(stmt, rc);

  rc= mysql_stmt_fetch(stmt);
  check_execute(stmt, rc);

  if (!opt_silent)
    fprintf(stdout, "\n row 1: %d", id);
  DIE_UNLESS(id == 1);

  rc= mysql_stmt_fetch(stmt);
  DIE_UNLESS(rc == MYSQL_NO_DATA);

  id= 8;
  rc= mysql_stmt_execute(stmt);
  check_execute(stmt, rc);

  rc= mysql_stmt_fetch(stmt);
  check_execute(stmt, rc);

  if (!opt_silent)
    fprintf(stdout, "\n row 1: %d", id);
  DIE_UNLESS(id == 0);

  rc= mysql_stmt_fetch(stmt);
  DIE_UNLESS(rc == MYSQL_NO_DATA);

  mysql_stmt_close(stmt);
  DBUG_VOID_RETURN;
}