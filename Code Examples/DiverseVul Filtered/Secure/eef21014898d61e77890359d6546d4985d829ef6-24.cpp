static void test_bug11909()
{
  MYSQL_STMT *stmt1, *stmt2;
  MYSQL_BIND my_bind[7];
  int rc;
  char firstname[20], midinit[20], lastname[20], workdept[20];
  ulong firstname_len, midinit_len, lastname_len, workdept_len;
  uint32 empno;
  double salary;
  float bonus;
  const char *stmt_text;

  myheader("test_bug11909");

  stmt_text= "drop table if exists t1";
  rc= mysql_real_query(mysql, stmt_text, strlen(stmt_text));
  myquery(rc);

  stmt_text= "create table t1 ("
    "  empno int(11) not null, firstname varchar(20) not null,"
    "  midinit varchar(20) not null, lastname varchar(20) not null,"
    "  workdept varchar(6) not null, salary double not null,"
    "  bonus float not null, primary key (empno)"
    ") default charset=latin1 collate=latin1_bin";
  rc= mysql_real_query(mysql, stmt_text, strlen(stmt_text));
  myquery(rc);

  stmt_text= "insert into t1 values "
    "(10, 'CHRISTINE', 'I', 'HAAS',     'A00', 52750, 1000), "
    "(20, 'MICHAEL',   'L', 'THOMPSON', 'B01', 41250, 800),"
    "(30, 'SALLY',     'A', 'KWAN',     'C01', 38250, 800),"
    "(50, 'JOHN',      'B', 'GEYER',    'E01', 40175, 800), "
    "(60, 'IRVING',    'F', 'STERN',    'D11', 32250, 500)";
  rc= mysql_real_query(mysql, stmt_text, strlen(stmt_text));
  myquery(rc);

  /* ****** Begin of trace ****** */

  stmt1= open_cursor("SELECT empno, firstname, midinit, lastname,"
                     "workdept, salary, bonus FROM t1");

  bzero(my_bind, sizeof(my_bind));
  my_bind[0].buffer_type= MYSQL_TYPE_LONG;
  my_bind[0].buffer= (void*) &empno;

  my_bind[1].buffer_type= MYSQL_TYPE_VAR_STRING;
  my_bind[1].buffer= (void*) firstname;
  my_bind[1].buffer_length= sizeof(firstname);
  my_bind[1].length= &firstname_len;

  my_bind[2].buffer_type= MYSQL_TYPE_VAR_STRING;
  my_bind[2].buffer= (void*) midinit;
  my_bind[2].buffer_length= sizeof(midinit);
  my_bind[2].length= &midinit_len;

  my_bind[3].buffer_type= MYSQL_TYPE_VAR_STRING;
  my_bind[3].buffer= (void*) lastname;
  my_bind[3].buffer_length= sizeof(lastname);
  my_bind[3].length= &lastname_len;

  my_bind[4].buffer_type= MYSQL_TYPE_VAR_STRING;
  my_bind[4].buffer= (void*) workdept;
  my_bind[4].buffer_length= sizeof(workdept);
  my_bind[4].length= &workdept_len;

  my_bind[5].buffer_type= MYSQL_TYPE_DOUBLE;
  my_bind[5].buffer= (void*) &salary;

  my_bind[6].buffer_type= MYSQL_TYPE_FLOAT;
  my_bind[6].buffer= (void*) &bonus;
  rc= mysql_stmt_bind_result(stmt1, my_bind);
  check_execute(stmt1, rc);

  rc= mysql_stmt_execute(stmt1);
  check_execute(stmt1, rc);

  rc= mysql_stmt_fetch(stmt1);
  DIE_UNLESS(rc == 0);
  DIE_UNLESS(empno == 10);
  DIE_UNLESS(strcmp(firstname, "CHRISTINE") == 0);
  DIE_UNLESS(strcmp(midinit, "I") == 0);
  DIE_UNLESS(strcmp(lastname, "HAAS") == 0);
  DIE_UNLESS(strcmp(workdept, "A00") == 0);
  DIE_UNLESS(salary == (double) 52750.0);
  DIE_UNLESS(bonus == (float) 1000.0);

  stmt2= open_cursor("SELECT empno, firstname FROM t1");
  rc= mysql_stmt_bind_result(stmt2, my_bind);
  check_execute(stmt2, rc);

  rc= mysql_stmt_execute(stmt2);
  check_execute(stmt2, rc);

  rc= mysql_stmt_fetch(stmt2);
  DIE_UNLESS(rc == 0);

  DIE_UNLESS(empno == 10);
  DIE_UNLESS(strcmp(firstname, "CHRISTINE") == 0);

  rc= mysql_stmt_reset(stmt2);
  check_execute(stmt2, rc);

  /* ERROR: next statement should return 0 */

  rc= mysql_stmt_fetch(stmt1);
  DIE_UNLESS(rc == 0);

  mysql_stmt_close(stmt1);
  mysql_stmt_close(stmt2);
  rc= mysql_rollback(mysql);
  myquery(rc);

  rc= mysql_query(mysql, "drop table t1");
  myquery(rc);
}