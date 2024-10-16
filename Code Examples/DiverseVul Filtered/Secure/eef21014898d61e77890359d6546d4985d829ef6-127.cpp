static void test_truncation()
{
  MYSQL_STMT *stmt;
  const char *stmt_text;
  int rc;
  uint bind_count;
  MYSQL_BIND *bind_array, *my_bind;

  myheader("test_truncation");

  /* Prepare the test table */
  rc= mysql_query(mysql, "drop table if exists t1");
  myquery(rc);

  stmt_text= "create table t1 ("
             "i8 tinyint, ui8 tinyint unsigned, "
             "i16 smallint, i16_1 smallint, "
             "ui16 smallint unsigned, i32 int, i32_1 int, "
             "d double, d_1 double, ch char(30), ch_1 char(30), "
             "tx text, tx_1 text, ch_2 char(30) "
             ")";
  rc= mysql_real_query(mysql, stmt_text, strlen(stmt_text));
  myquery(rc);

  {
    const char insert_text[]= 
             "insert into t1 VALUES ("
             "-10, "                            /* i8 */
             "200, "                            /* ui8 */
             "32000, "                          /* i16 */
             "-32767, "                         /* i16_1 */
             "64000, "                          /* ui16 */
             "1073741824, "                     /* i32 */
             "1073741825, "                     /* i32_1 */
             "123.456, "                        /* d */
             "-12345678910, "                   /* d_1 */
             "'111111111111111111111111111111',"/* ch */
             "'abcdef', "                       /* ch_1 */
             "'12345 	      ', "              /* tx */
             "'12345.67 	      ', "      /* tx_1 */
             "'12345.67abc'"                    /* ch_2 */
             ")";
    rc= mysql_real_query(mysql, insert_text, strlen(insert_text));
    myquery(rc);
  }

  stmt_text= "select i8 c1, i8 c2, ui8 c3, i16_1 c4, ui16 c5, "
             "       i16 c6, ui16 c7, i32 c8, i32_1 c9, i32_1 c10, "
             "       d c11, d_1 c12, d_1 c13, ch c14, ch_1 c15, tx c16, "
             "       tx_1 c17, ch_2 c18 "
             "from t1";

  stmt= mysql_stmt_init(mysql);
  rc= mysql_stmt_prepare(stmt, stmt_text, strlen(stmt_text));
  check_execute(stmt, rc);
  rc= mysql_stmt_execute(stmt);
  check_execute(stmt, rc);
  bind_count= (uint) mysql_stmt_field_count(stmt);

  /*************** Fill in the bind structure and bind it **************/
  bind_array= malloc(sizeof(MYSQL_BIND) * bind_count);
  bzero((char*) bind_array, sizeof(MYSQL_BIND) * bind_count);
  for (my_bind= bind_array; my_bind < bind_array + bind_count; my_bind++)
    my_bind->error= &my_bind->error_value;
  my_bind= bind_array;

  my_bind->buffer= malloc(sizeof(uint8));
  my_bind->buffer_type= MYSQL_TYPE_TINY;
  my_bind->is_unsigned= TRUE;

  DIE_UNLESS(my_bind++ < bind_array + bind_count);
  my_bind->buffer= malloc(sizeof(uint32));
  my_bind->buffer_type= MYSQL_TYPE_LONG;
  my_bind->is_unsigned= TRUE;

  DIE_UNLESS(my_bind++ < bind_array + bind_count);
  my_bind->buffer= malloc(sizeof(int8));
  my_bind->buffer_type= MYSQL_TYPE_TINY;

  DIE_UNLESS(my_bind++ < bind_array + bind_count);
  my_bind->buffer= malloc(sizeof(uint16));
  my_bind->buffer_type= MYSQL_TYPE_SHORT;
  my_bind->is_unsigned= TRUE;

  DIE_UNLESS(my_bind++ < bind_array + bind_count);
  my_bind->buffer= malloc(sizeof(int16));
  my_bind->buffer_type= MYSQL_TYPE_SHORT;

  DIE_UNLESS(my_bind++ < bind_array + bind_count);
  my_bind->buffer= malloc(sizeof(uint16));
  my_bind->buffer_type= MYSQL_TYPE_SHORT;
  my_bind->is_unsigned= TRUE;

  DIE_UNLESS(my_bind++ < bind_array + bind_count);
  my_bind->buffer= malloc(sizeof(int8));
  my_bind->buffer_type= MYSQL_TYPE_TINY;
  my_bind->is_unsigned= TRUE;

  DIE_UNLESS(my_bind++ < bind_array + bind_count);
  my_bind->buffer= malloc(sizeof(float));
  my_bind->buffer_type= MYSQL_TYPE_FLOAT;

  DIE_UNLESS(my_bind++ < bind_array + bind_count);
  my_bind->buffer= malloc(sizeof(float));
  my_bind->buffer_type= MYSQL_TYPE_FLOAT;

  DIE_UNLESS(my_bind++ < bind_array + bind_count);
  my_bind->buffer= malloc(sizeof(double));
  my_bind->buffer_type= MYSQL_TYPE_DOUBLE;

  DIE_UNLESS(my_bind++ < bind_array + bind_count);
  my_bind->buffer= malloc(sizeof(longlong));
  my_bind->buffer_type= MYSQL_TYPE_LONGLONG;

  DIE_UNLESS(my_bind++ < bind_array + bind_count);
  my_bind->buffer= malloc(sizeof(ulonglong));
  my_bind->buffer_type= MYSQL_TYPE_LONGLONG;
  my_bind->is_unsigned= TRUE;

  DIE_UNLESS(my_bind++ < bind_array + bind_count);
  my_bind->buffer= malloc(sizeof(longlong));
  my_bind->buffer_type= MYSQL_TYPE_LONGLONG;

  DIE_UNLESS(my_bind++ < bind_array + bind_count);
  my_bind->buffer= malloc(sizeof(longlong));
  my_bind->buffer_type= MYSQL_TYPE_LONGLONG;

  DIE_UNLESS(my_bind++ < bind_array + bind_count);
  my_bind->buffer= malloc(sizeof(longlong));
  my_bind->buffer_type= MYSQL_TYPE_LONGLONG;

  DIE_UNLESS(my_bind++ < bind_array + bind_count);
  my_bind->buffer= malloc(sizeof(longlong));
  my_bind->buffer_type= MYSQL_TYPE_LONGLONG;

  DIE_UNLESS(my_bind++ < bind_array + bind_count);
  my_bind->buffer= malloc(sizeof(double));
  my_bind->buffer_type= MYSQL_TYPE_DOUBLE;

  DIE_UNLESS(my_bind++ < bind_array + bind_count);
  my_bind->buffer= malloc(sizeof(double));
  my_bind->buffer_type= MYSQL_TYPE_DOUBLE;

  rc= mysql_stmt_bind_result(stmt, bind_array);
  check_execute(stmt, rc);
  rc= mysql_stmt_fetch(stmt);
  DIE_UNLESS(rc == MYSQL_DATA_TRUNCATED);

  /*************** Verify truncation results ***************************/
  my_bind= bind_array;

  /* signed tiny -> tiny */
  DIE_UNLESS(*my_bind->error && * (int8*) my_bind->buffer == -10);

  /* signed tiny -> uint32 */
  DIE_UNLESS(my_bind++ < bind_array + bind_count);
  DIE_UNLESS(*my_bind->error && * (int32*) my_bind->buffer == -10);

  /* unsigned tiny -> tiny */
  DIE_UNLESS(my_bind++ < bind_array + bind_count);
  DIE_UNLESS(*my_bind->error && * (uint8*) my_bind->buffer == 200);

  /* short -> ushort */
  DIE_UNLESS(my_bind++ < bind_array + bind_count);
  DIE_UNLESS(*my_bind->error && * (int16*) my_bind->buffer == -32767);

  /* ushort -> short */
  DIE_UNLESS(my_bind++ < bind_array + bind_count);
  DIE_UNLESS(*my_bind->error && * (uint16*) my_bind->buffer == 64000);

  /* short -> ushort (no truncation, data is in the range of target type) */
  DIE_UNLESS(my_bind++ < bind_array + bind_count);
  DIE_UNLESS(! *my_bind->error && * (uint16*) my_bind->buffer == 32000);

  /* ushort -> utiny */
  DIE_UNLESS(my_bind++ < bind_array + bind_count);
  DIE_UNLESS(*my_bind->error && * (int8*) my_bind->buffer == 0);

  /* int -> float: no truncation, the number is a power of two */
  DIE_UNLESS(my_bind++ < bind_array + bind_count);
  DIE_UNLESS(! *my_bind->error && * (float*) my_bind->buffer == 1073741824);

  /* int -> float: truncation, not enough bits in float */
  DIE_UNLESS(my_bind++ < bind_array + bind_count);
  DIE_UNLESS(*my_bind->error);

  /* int -> double: no truncation */
  DIE_UNLESS(my_bind++ < bind_array + bind_count);
  DIE_UNLESS(! *my_bind->error && * (double*) my_bind->buffer == 1073741825);

  /* double -> longlong: fractional part is lost */
  DIE_UNLESS(my_bind++ < bind_array + bind_count);

  /* double -> ulonglong, negative fp number to unsigned integer */
  DIE_UNLESS(my_bind++ < bind_array + bind_count);
  /* Value in the buffer is not defined: don't test it */
  DIE_UNLESS(*my_bind->error);

  /* double -> longlong, negative fp number to signed integer: no loss */
  DIE_UNLESS(my_bind++ < bind_array + bind_count);
  DIE_UNLESS(! *my_bind->error && * (longlong*) my_bind->buffer == LL(-12345678910));

  /* big numeric string -> number */
  DIE_UNLESS(my_bind++ < bind_array + bind_count);
  DIE_UNLESS(*my_bind->error);

  /* junk string -> number */
  DIE_UNLESS(my_bind++ < bind_array + bind_count);
  DIE_UNLESS(*my_bind->error && *(longlong*) my_bind->buffer == 0);

  /* string with trailing spaces -> number */
  DIE_UNLESS(my_bind++ < bind_array + bind_count);
  DIE_UNLESS(! *my_bind->error && *(longlong*) my_bind->buffer == 12345);

  /* string with trailing spaces -> double */
  DIE_UNLESS(my_bind++ < bind_array + bind_count);
  DIE_UNLESS(! *my_bind->error && *(double*) my_bind->buffer == 12345.67);

  /* string with trailing junk -> double */
  DIE_UNLESS(my_bind++ < bind_array + bind_count);
  /*
    XXX: There must be a truncation error: but it's not the way the server
    behaves, so let's leave it for now.
  */
  DIE_UNLESS(*(double*) my_bind->buffer == 12345.67);
  /*
    TODO: string -> double,  double -> time, double -> string (truncation
          errors are not supported here yet)
          longlong -> time/date/datetime
          date -> time, date -> timestamp, date -> number
          time -> string, time -> date, time -> timestamp,
          number -> date string -> date
  */
  /*************** Cleanup *********************************************/

  mysql_stmt_close(stmt);

  for (my_bind= bind_array; my_bind < bind_array + bind_count; my_bind++)
    free(my_bind->buffer);
  free(bind_array);

  rc= mysql_query(mysql, "drop table t1");
  myquery(rc);
}