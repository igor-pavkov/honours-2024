static void test_bug3035()
{
  MYSQL_STMT *stmt;
  int rc;
  MYSQL_BIND bind_array[12], *my_bind= bind_array, *bind_end= my_bind + 12;
  int8 int8_val;
  uint8 uint8_val;
  int16 int16_val;
  uint16 uint16_val;
  int32 int32_val;
  uint32 uint32_val;
  longlong int64_val;
  ulonglong uint64_val;
  double double_val, udouble_val, double_tmp;
  char longlong_as_string[22], ulonglong_as_string[22];

  /* mins and maxes */
  const int8 int8_min= -128;
  const int8 int8_max= 127;
  const uint8 uint8_min= 0;
  const uint8 uint8_max= 255;

  const int16 int16_min= -32768;
  const int16 int16_max= 32767;
  const uint16 uint16_min= 0;
  const uint16 uint16_max= 65535;

  const int32 int32_max= 2147483647L;
  const int32 int32_min= -int32_max - 1;
  const uint32 uint32_min= 0;
  const uint32 uint32_max= 4294967295U;

  /* it might not work okay everyplace */
  const longlong int64_max= LL(9223372036854775807);
  const longlong int64_min= -int64_max - 1;

  const ulonglong uint64_min= 0U;
  const ulonglong uint64_max= ULL(18446744073709551615);

  const char *stmt_text;

  myheader("test_bug3035");

  stmt_text= "DROP TABLE IF EXISTS t1";
  rc= mysql_real_query(mysql, stmt_text, strlen(stmt_text));
  myquery(rc);

  stmt_text= "CREATE TABLE t1 (i8 TINYINT, ui8 TINYINT UNSIGNED, "
                              "i16 SMALLINT, ui16 SMALLINT UNSIGNED, "
                              "i32 INT, ui32 INT UNSIGNED, "
                              "i64 BIGINT, ui64 BIGINT UNSIGNED, "
                              "id INTEGER NOT NULL PRIMARY KEY AUTO_INCREMENT)";
  rc= mysql_real_query(mysql, stmt_text, strlen(stmt_text));
  myquery(rc);

  bzero((char*) bind_array, sizeof(bind_array));

  for (my_bind= bind_array; my_bind < bind_end; my_bind++)
    my_bind->error= &my_bind->error_value;

  bind_array[0].buffer_type= MYSQL_TYPE_TINY;
  bind_array[0].buffer= (void *) &int8_val;

  bind_array[1].buffer_type= MYSQL_TYPE_TINY;
  bind_array[1].buffer= (void *) &uint8_val;
  bind_array[1].is_unsigned= 1;

  bind_array[2].buffer_type= MYSQL_TYPE_SHORT;
  bind_array[2].buffer= (void *) &int16_val;

  bind_array[3].buffer_type= MYSQL_TYPE_SHORT;
  bind_array[3].buffer= (void *) &uint16_val;
  bind_array[3].is_unsigned= 1;

  bind_array[4].buffer_type= MYSQL_TYPE_LONG;
  bind_array[4].buffer= (void *) &int32_val;

  bind_array[5].buffer_type= MYSQL_TYPE_LONG;
  bind_array[5].buffer= (void *) &uint32_val;
  bind_array[5].is_unsigned= 1;

  bind_array[6].buffer_type= MYSQL_TYPE_LONGLONG;
  bind_array[6].buffer= (void *) &int64_val;

  bind_array[7].buffer_type= MYSQL_TYPE_LONGLONG;
  bind_array[7].buffer= (void *) &uint64_val;
  bind_array[7].is_unsigned= 1;

  stmt= mysql_stmt_init(mysql);
  check_stmt(stmt);

  stmt_text= "INSERT INTO t1 (i8, ui8, i16, ui16, i32, ui32, i64, ui64) "
                     "VALUES (?, ?, ?, ?, ?, ?, ?, ?)";
  rc= mysql_stmt_prepare(stmt, stmt_text, strlen(stmt_text));
  check_execute(stmt, rc);

  mysql_stmt_bind_param(stmt, bind_array);

  int8_val= int8_min;
  uint8_val= uint8_min;
  int16_val= int16_min;
  uint16_val= uint16_min;
  int32_val= int32_min;
  uint32_val= uint32_min;
  int64_val= int64_min;
  uint64_val= uint64_min;

  rc= mysql_stmt_execute(stmt);
  check_execute(stmt, rc);

  int8_val= int8_max;
  uint8_val= uint8_max;
  int16_val= int16_max;
  uint16_val= uint16_max;
  int32_val= int32_max;
  uint32_val= uint32_max;
  int64_val= int64_max;
  uint64_val= uint64_max;

  rc= mysql_stmt_execute(stmt);
  check_execute(stmt, rc);

  stmt_text= "SELECT i8, ui8, i16, ui16, i32, ui32, i64, ui64, ui64, "
             "cast(ui64 as signed), ui64, cast(ui64 as signed)"
             "FROM t1 ORDER BY id ASC";

  rc= mysql_stmt_prepare(stmt, stmt_text, strlen(stmt_text));
  check_execute(stmt, rc);

  rc= mysql_stmt_execute(stmt);
  check_execute(stmt, rc);

  bind_array[8].buffer_type= MYSQL_TYPE_DOUBLE;
  bind_array[8].buffer= (void *) &udouble_val;

  bind_array[9].buffer_type= MYSQL_TYPE_DOUBLE;
  bind_array[9].buffer= (void *) &double_val;

  bind_array[10].buffer_type= MYSQL_TYPE_STRING;
  bind_array[10].buffer= (void *) &ulonglong_as_string;
  bind_array[10].buffer_length= sizeof(ulonglong_as_string);

  bind_array[11].buffer_type= MYSQL_TYPE_STRING;
  bind_array[11].buffer= (void *) &longlong_as_string;
  bind_array[11].buffer_length= sizeof(longlong_as_string);

  mysql_stmt_bind_result(stmt, bind_array);

  rc= mysql_stmt_fetch(stmt);
  check_execute(stmt, rc);

  DIE_UNLESS(int8_val == int8_min);
  DIE_UNLESS(uint8_val == uint8_min);
  DIE_UNLESS(int16_val == int16_min);
  DIE_UNLESS(uint16_val == uint16_min);
  DIE_UNLESS(int32_val == int32_min);
  DIE_UNLESS(uint32_val == uint32_min);
  DIE_UNLESS(int64_val == int64_min);
  DIE_UNLESS(uint64_val == uint64_min);
  DIE_UNLESS(double_val == (longlong) uint64_min);
  double_tmp= ulonglong2double(uint64_val);
  DIE_UNLESS(cmp_double(&udouble_val, &double_tmp));
  DIE_UNLESS(!strcmp(longlong_as_string, "0"));
  DIE_UNLESS(!strcmp(ulonglong_as_string, "0"));

  rc= mysql_stmt_fetch(stmt);

  if (!opt_silent)
  {
    printf("Truncation mask: ");
    for (my_bind= bind_array; my_bind < bind_end; my_bind++)
      printf("%d", (int) my_bind->error_value);
    printf("\n");
  }
  DIE_UNLESS(rc == MYSQL_DATA_TRUNCATED || rc == 0);

  DIE_UNLESS(int8_val == int8_max);
  DIE_UNLESS(uint8_val == uint8_max);
  DIE_UNLESS(int16_val == int16_max);
  DIE_UNLESS(uint16_val == uint16_max);
  DIE_UNLESS(int32_val == int32_max);
  DIE_UNLESS(uint32_val == uint32_max);
  DIE_UNLESS(int64_val == int64_max);
  DIE_UNLESS(uint64_val == uint64_max);
  DIE_UNLESS(double_val == (longlong) uint64_val);
  double_tmp= ulonglong2double(uint64_val);
  DIE_UNLESS(cmp_double(&udouble_val, &double_tmp));
  DIE_UNLESS(!strcmp(longlong_as_string, "-1"));
  DIE_UNLESS(!strcmp(ulonglong_as_string, "18446744073709551615"));

  rc= mysql_stmt_fetch(stmt);
  DIE_UNLESS(rc == MYSQL_NO_DATA);

  mysql_stmt_close(stmt);

  stmt_text= "DROP TABLE t1";
  mysql_real_query(mysql, stmt_text, strlen(stmt_text));
}