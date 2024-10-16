static void test_bug1500()
{
  MYSQL_STMT *stmt;
  MYSQL_BIND my_bind[3];
  int        rc;
  int32 int_data[3]= {2, 3, 4};
  const char *data;

  myheader("test_bug1500");

  rc= mysql_query(mysql, "DROP TABLE IF EXISTS test_bg1500");
  myquery(rc);

  rc= mysql_query(mysql, "CREATE TABLE test_bg1500 (i INT)");
  myquery(rc);

  rc= mysql_query(mysql, "INSERT INTO test_bg1500 VALUES (1), (2)");
  myquery(rc);

  rc= mysql_commit(mysql);
  myquery(rc);

  stmt= mysql_simple_prepare(mysql, "SELECT i FROM test_bg1500 WHERE i IN (?, ?, ?)");
  check_stmt(stmt);
  verify_param_count(stmt, 3);

  /*
    We need to bzero bind structure because mysql_stmt_bind_param checks all
    its members.
  */
  bzero((char*) my_bind, sizeof(my_bind));

  my_bind[0].buffer= (void *)int_data;
  my_bind[0].buffer_type= MYSQL_TYPE_LONG;
  my_bind[2]= my_bind[1]= my_bind[0];
  my_bind[1].buffer= (void *)(int_data + 1);
  my_bind[2].buffer= (void *)(int_data + 2);

  rc= mysql_stmt_bind_param(stmt, my_bind);
  check_execute(stmt, rc);

  rc= mysql_stmt_execute(stmt);
  check_execute(stmt, rc);

  rc= my_process_stmt_result(stmt);
  DIE_UNLESS(rc == 1);

  mysql_stmt_close(stmt);

  rc= mysql_query(mysql, "DROP TABLE test_bg1500");
  myquery(rc);

  rc= mysql_query(mysql, "CREATE TABLE test_bg1500 (s VARCHAR(25), FULLTEXT(s)) engine=MyISAM");
  myquery(rc);

  rc= mysql_query(mysql,
        "INSERT INTO test_bg1500 VALUES ('Gravedigger'), ('Greed'), ('Hollow Dogs')");
  myquery(rc);

  rc= mysql_commit(mysql);
  myquery(rc);

  stmt= mysql_simple_prepare(mysql,
          "SELECT s FROM test_bg1500 WHERE MATCH (s) AGAINST (?)");
  check_stmt(stmt);

  verify_param_count(stmt, 1);

  data= "Dogs";
  my_bind[0].buffer_type= MYSQL_TYPE_STRING;
  my_bind[0].buffer= (void *) data;
  my_bind[0].buffer_length= strlen(data);
  my_bind[0].is_null= 0;
  my_bind[0].length= 0;

  rc= mysql_stmt_bind_param(stmt, my_bind);
  check_execute(stmt, rc);

  rc= mysql_stmt_execute(stmt);
  check_execute(stmt, rc);

  rc= my_process_stmt_result(stmt);
  DIE_UNLESS(rc == 1);

  mysql_stmt_close(stmt);

  /* This should work too */
  stmt= mysql_simple_prepare(mysql,
          "SELECT s FROM test_bg1500 WHERE MATCH (s) AGAINST (CONCAT(?, 'digger'))");
  check_stmt(stmt);

  verify_param_count(stmt, 1);

  data= "Grave";
  my_bind[0].buffer_type= MYSQL_TYPE_STRING;
  my_bind[0].buffer= (void *) data;
  my_bind[0].buffer_length= strlen(data);

  rc= mysql_stmt_bind_param(stmt, my_bind);
  check_execute(stmt, rc);

  rc= mysql_stmt_execute(stmt);
  check_execute(stmt, rc);

  rc= my_process_stmt_result(stmt);
  DIE_UNLESS(rc == 1);

  mysql_stmt_close(stmt);
}