static void test_bug11901()
{
  MYSQL_STMT *stmt;
  MYSQL_BIND my_bind[2];
  int rc;
  char workdept[20];
  ulong workdept_len;
  uint32 empno;
  const char *stmt_text;

  myheader("test_bug11901");

  stmt_text= "drop table if exists t1, t2";
  rc= mysql_real_query(mysql, stmt_text, strlen(stmt_text));
  myquery(rc);

  stmt_text= "create table t1 ("
    "  empno int(11) not null, firstname varchar(20) not null,"
    "  midinit varchar(20) not null, lastname varchar(20) not null,"
    "  workdept varchar(6) not null, salary double not null,"
    "  bonus float not null, primary key (empno), "
    " unique key (workdept, empno) "
    ") default charset=latin1 collate=latin1_bin";
  rc= mysql_real_query(mysql, stmt_text, strlen(stmt_text));
  myquery(rc);

  stmt_text= "insert into t1 values "
     "(10,  'CHRISTINE', 'I', 'HAAS',      'A00', 52750, 1000),"
     "(20,  'MICHAEL',   'L', 'THOMPSON',  'B01', 41250, 800), "
     "(30,  'SALLY',     'A', 'KWAN',      'C01', 38250, 800), "
     "(50,  'JOHN',      'B', 'GEYER',     'E01', 40175, 800), "
     "(60,  'IRVING',    'F', 'STERN',     'D11', 32250, 500), "
     "(70,  'EVA',       'D', 'PULASKI',   'D21', 36170, 700), "
     "(90,  'EILEEN',    'W', 'HENDERSON', 'E11', 29750, 600), "
     "(100, 'THEODORE',  'Q', 'SPENSER',   'E21', 26150, 500), "
     "(110, 'VINCENZO',  'G', 'LUCCHESSI', 'A00', 46500, 900), "
     "(120, 'SEAN',      '',  'O\\'CONNELL', 'A00', 29250, 600), "
     "(130, 'DOLORES',   'M', 'QUINTANA',  'C01', 23800, 500), "
     "(140, 'HEATHER',   'A', 'NICHOLLS',  'C01', 28420, 600), "
     "(150, 'BRUCE',     '',  'ADAMSON',   'D11', 25280, 500), "
     "(160, 'ELIZABETH', 'R', 'PIANKA',    'D11', 22250, 400), "
     "(170, 'MASATOSHI', 'J', 'YOSHIMURA', 'D11', 24680, 500), "
     "(180, 'MARILYN',   'S', 'SCOUTTEN',  'D11', 21340, 500), "
     "(190, 'JAMES',     'H', 'WALKER',    'D11', 20450, 400), "
     "(200, 'DAVID',     '',  'BROWN',     'D11', 27740, 600), "
     "(210, 'WILLIAM',   'T', 'JONES',     'D11', 18270, 400), "
     "(220, 'JENNIFER',  'K', 'LUTZ',      'D11', 29840, 600), "
     "(230, 'JAMES',     'J', 'JEFFERSON', 'D21', 22180, 400), "
     "(240, 'SALVATORE', 'M', 'MARINO',    'D21', 28760, 600), "
     "(250, 'DANIEL',    'S', 'SMITH',     'D21', 19180, 400), "
     "(260, 'SYBIL',     'P', 'JOHNSON',   'D21', 17250, 300), "
     "(270, 'MARIA',     'L', 'PEREZ',     'D21', 27380, 500), "
     "(280, 'ETHEL',     'R', 'SCHNEIDER', 'E11', 26250, 500), "
     "(290, 'JOHN',      'R', 'PARKER',    'E11', 15340, 300), "
     "(300, 'PHILIP',    'X', 'SMITH',     'E11', 17750, 400), "
     "(310, 'MAUDE',     'F', 'SETRIGHT',  'E11', 15900, 300), "
     "(320, 'RAMLAL',    'V', 'MEHTA',     'E21', 19950, 400), "
     "(330, 'WING',      '',  'LEE',       'E21', 25370, 500), "
     "(340, 'JASON',     'R', 'GOUNOT',    'E21', 23840, 500)";

  rc= mysql_real_query(mysql, stmt_text, strlen(stmt_text));
  myquery(rc);

  stmt_text= "create table t2 ("
    " deptno varchar(6) not null, deptname varchar(20) not null,"
    " mgrno int(11) not null, location varchar(20) not null,"
    " admrdept varchar(6) not null, refcntd int(11) not null,"
    " refcntu int(11) not null, primary key (deptno)"
    ") default charset=latin1 collate=latin1_bin";
  rc= mysql_real_query(mysql, stmt_text, strlen(stmt_text));
  myquery(rc);

  stmt_text= "insert into t2 values "
    "('A00', 'SPIFFY COMPUTER SERV', 10, '', 'A00', 0, 0), "
    "('B01', 'PLANNING',             20, '', 'A00', 0, 0), "
    "('C01', 'INFORMATION CENTER',   30, '', 'A00', 0, 0), "
    "('D01', 'DEVELOPMENT CENTER',   0,  '', 'A00', 0, 0),"
    "('D11', 'MANUFACTURING SYSTEM', 60, '', 'D01', 0, 0), "
    "('D21', 'ADMINISTRATION SYSTE', 70, '', 'D01', 0, 0), "
    "('E01', 'SUPPORT SERVICES',     50, '', 'A00', 0, 0), "
    "('E11', 'OPERATIONS',           90, '', 'E01', 0, 0), "
    "('E21', 'SOFTWARE SUPPORT',     100,'', 'E01', 0, 0)";
  rc= mysql_real_query(mysql, stmt_text, strlen(stmt_text));
  myquery(rc);

  /* ****** Begin of trace ****** */

  stmt= open_cursor("select t1.empno, t1.workdept "
                    "from (t1 left join t2 on t2.deptno = t1.workdept) "
                    "where t2.deptno in "
                    "   (select t2.deptno "
                    "    from (t1 left join t2 on t2.deptno = t1.workdept) "
                    "    where t1.empno = ?) "
                    "order by 1");
  bzero(my_bind, sizeof(my_bind));

  my_bind[0].buffer_type= MYSQL_TYPE_LONG;
  my_bind[0].buffer= &empno;
  rc= mysql_stmt_bind_param(stmt, my_bind);
  check_execute(stmt, rc);

  my_bind[1].buffer_type= MYSQL_TYPE_VAR_STRING;
  my_bind[1].buffer= (void*) workdept;
  my_bind[1].buffer_length= sizeof(workdept);
  my_bind[1].length= &workdept_len;

  rc= mysql_stmt_bind_result(stmt, my_bind);
  check_execute(stmt, rc);

  empno= 10;
  /* ERROR: next statement causes a server crash */
  rc= mysql_stmt_execute(stmt);
  check_execute(stmt, rc);

  mysql_stmt_close(stmt);

  rc= mysql_query(mysql, "drop table t1, t2");
  myquery(rc);
}