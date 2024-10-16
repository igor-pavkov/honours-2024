static void test_bind_date_conv(uint row_count)
{
  MYSQL_STMT   *stmt= 0;
  uint         rc, i, count= row_count;
  ulong        length[4];
  MYSQL_BIND   my_bind[4];
  my_bool      is_null[4]= {0};
  MYSQL_TIME   tm[4];
  ulong        second_part;
  uint         year, month, day, hour, minute, sec;

  stmt= mysql_simple_prepare(mysql, "INSERT INTO test_date VALUES(?, ?, ?, ?)");
  check_stmt(stmt);

  verify_param_count(stmt, 4);

  /*
    We need to bzero bind structure because mysql_stmt_bind_param checks all
    its members.
  */
  bzero((char*) my_bind, sizeof(my_bind));

  my_bind[0].buffer_type= MYSQL_TYPE_TIMESTAMP;
  my_bind[1].buffer_type= MYSQL_TYPE_TIME;
  my_bind[2].buffer_type= MYSQL_TYPE_DATETIME;
  my_bind[3].buffer_type= MYSQL_TYPE_DATE;

  for (i= 0; i < (int) array_elements(my_bind); i++)
  {
    my_bind[i].buffer= (void *) &tm[i];
    my_bind[i].is_null= &is_null[i];
    my_bind[i].length= &length[i];
    my_bind[i].buffer_length= 30;
    length[i]= 20;
  }

  second_part= 0;

  year= 2000;
  month= 01;
  day= 10;

  hour= 11;
  minute= 16;
  sec= 20;

  rc= mysql_stmt_bind_param(stmt, my_bind);
  check_execute(stmt, rc);

  for (count= 0; count < row_count; count++)
  {
    for (i= 0; i < (int) array_elements(my_bind); i++)
    {
      tm[i].neg= 0;
      tm[i].second_part= second_part+count;
      if (my_bind[i].buffer_type != MYSQL_TYPE_TIME)
      {
        tm[i].year= year+count;
        tm[i].month= month+count;
        tm[i].day= day+count;
      }
      else
        tm[i].year= tm[i].month= tm[i].day= 0;
      if (my_bind[i].buffer_type != MYSQL_TYPE_DATE)
      {
        tm[i].hour= hour+count;
        tm[i].minute= minute+count;
        tm[i].second= sec+count;
      }
      else
        tm[i].hour= tm[i].minute= tm[i].second= 0;
    }
    rc= mysql_stmt_execute(stmt);
    check_execute(stmt, rc);
  }

  rc= mysql_commit(mysql);
  myquery(rc);

  mysql_stmt_close(stmt);

  rc= my_stmt_result("SELECT * FROM test_date");
  DIE_UNLESS(row_count == rc);

  stmt= mysql_simple_prepare(mysql, "SELECT * FROM test_date");
  check_stmt(stmt);

  rc= mysql_stmt_bind_result(stmt, my_bind);
  check_execute(stmt, rc);

  rc= mysql_stmt_execute(stmt);
  check_execute(stmt, rc);

  rc= mysql_stmt_store_result(stmt);
  check_execute(stmt, rc);

  for (count= 0; count < row_count; count++)
  {
    rc= mysql_stmt_fetch(stmt);
    DIE_UNLESS(rc == 0 || rc == MYSQL_DATA_TRUNCATED);

    if (!opt_silent)
      fprintf(stdout, "\n");
    for (i= 0; i < array_elements(my_bind); i++)
    {
      if (!opt_silent)
        fprintf(stdout, "\ntime[%d]: %02d-%02d-%02d %02d:%02d:%02d.%02lu",
                i, tm[i].year, tm[i].month, tm[i].day,
                tm[i].hour, tm[i].minute, tm[i].second,
                tm[i].second_part);
      DIE_UNLESS(tm[i].year == 0 || tm[i].year == year+count);
      DIE_UNLESS(tm[i].month == 0 || tm[i].month == month+count);
      DIE_UNLESS(tm[i].day == 0 || tm[i].day == day+count);

      DIE_UNLESS(tm[i].hour == 0 || tm[i].hour == hour+count);
      DIE_UNLESS(tm[i].minute == 0 || tm[i].minute == minute+count);
      DIE_UNLESS(tm[i].second == 0 || tm[i].second == sec+count);
      DIE_UNLESS(tm[i].second_part == 0 ||
                 tm[i].second_part == second_part+count);
    }
  }
  rc= mysql_stmt_fetch(stmt);
  DIE_UNLESS(rc == MYSQL_NO_DATA);

  mysql_stmt_close(stmt);
}