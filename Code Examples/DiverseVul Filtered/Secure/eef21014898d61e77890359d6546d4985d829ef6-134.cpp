static void test_wl4435_3()
{
  char tmp[255];

  puts("");

  /*
  // The following types are not supported:
  //   - ENUM
  //   - SET
  //
  // The following types are supported but can not be used for
  // OUT-parameters:
  //   - MEDIUMINT;
  //   - BIT(..);
  //
  // The problem is that those types are not supported for IN-parameters,
  // and OUT-parameters should be bound as IN-parameters before execution.
  //
  // The following types should not be used:
  //   - MYSQL_TYPE_YEAR (use MYSQL_TYPE_SHORT instead);
  //   - MYSQL_TYPE_TINY_BLOB, MYSQL_TYPE_MEDIUM_BLOB, MYSQL_TYPE_LONG_BLOB
  //     (use MYSQL_TYPE_BLOB instead);
  */

  WL4435_TEST("TINYINT", "127",
              MYSQL_TYPE_TINY, MYSQL_TYPE_TINY,
              char, ,
              ("  - TINYINT / char / MYSQL_TYPE_TINY:\t\t\t %d", (int) pspv),
              pspv == 127);

  WL4435_TEST("SMALLINT", "32767",
              MYSQL_TYPE_SHORT, MYSQL_TYPE_SHORT,
              short, ,
              ("  - SMALLINT / short / MYSQL_TYPE_SHORT:\t\t %d", (int) pspv),
              pspv == 32767);

  WL4435_TEST("INT", "2147483647",
              MYSQL_TYPE_LONG, MYSQL_TYPE_LONG,
              int, ,
              ("  - INT / int / MYSQL_TYPE_LONG:\t\t\t %d", pspv),
              pspv == 2147483647l);

  WL4435_TEST("BIGINT", "9223372036854775807",
              MYSQL_TYPE_LONGLONG, MYSQL_TYPE_LONGLONG,
              long long, ,
              ("  - BIGINT / long long / MYSQL_TYPE_LONGLONG:\t\t %lld", pspv),
              pspv == 9223372036854775807ll);

  WL4435_TEST("TIMESTAMP", "'2007-11-18 15:01:02'",
              MYSQL_TYPE_TIMESTAMP, MYSQL_TYPE_TIMESTAMP,
              MYSQL_TIME, ,
              ("  - TIMESTAMP / MYSQL_TIME / MYSQL_TYPE_TIMESTAMP:\t "
               "%.4d-%.2d-%.2d %.2d:%.2d:%.2d",
               (int) pspv.year, (int) pspv.month, (int) pspv.day,
               (int) pspv.hour, (int) pspv.minute, (int) pspv.second),
              pspv.year == 2007 && pspv.month == 11 && pspv.day == 18 &&
              pspv.hour == 15 && pspv.minute == 1 && pspv.second == 2);

  WL4435_TEST("DATETIME", "'1234-11-12 12:34:59'",
              MYSQL_TYPE_DATETIME, MYSQL_TYPE_DATETIME,
              MYSQL_TIME, ,
              ("  - DATETIME / MYSQL_TIME / MYSQL_TYPE_DATETIME:\t "
               "%.4d-%.2d-%.2d %.2d:%.2d:%.2d",
               (int) pspv.year, (int) pspv.month, (int) pspv.day,
               (int) pspv.hour, (int) pspv.minute, (int) pspv.second),
              pspv.year == 1234 && pspv.month == 11 && pspv.day == 12 &&
              pspv.hour == 12 && pspv.minute == 34 && pspv.second == 59);

  WL4435_TEST("TIME", "'123:45:01'",
              MYSQL_TYPE_TIME, MYSQL_TYPE_TIME,
              MYSQL_TIME, ,
              ("  - TIME / MYSQL_TIME / MYSQL_TYPE_TIME:\t\t "
               "%.3d:%.2d:%.2d",
               (int) pspv.hour, (int) pspv.minute, (int) pspv.second),
              pspv.hour == 123 && pspv.minute == 45 && pspv.second == 1);

  WL4435_TEST("DATE", "'1234-11-12'",
              MYSQL_TYPE_DATE, MYSQL_TYPE_DATE,
              MYSQL_TIME, ,
              ("  - DATE / MYSQL_TIME / MYSQL_TYPE_DATE:\t\t "
               "%.4d-%.2d-%.2d",
               (int) pspv.year, (int) pspv.month, (int) pspv.day),
              pspv.year == 1234 && pspv.month == 11 && pspv.day == 12);

  WL4435_TEST("YEAR", "'2010'",
              MYSQL_TYPE_SHORT, MYSQL_TYPE_YEAR,
              short, ,
              ("  - YEAR / short / MYSQL_TYPE_SHORT:\t\t\t %.4d", (int) pspv),
              pspv == 2010);

  WL4435_TEST("FLOAT(7, 4)", "123.4567",
              MYSQL_TYPE_FLOAT, MYSQL_TYPE_FLOAT,
              float, ,
              ("  - FLOAT / float / MYSQL_TYPE_FLOAT:\t\t\t %g", (double) pspv),
              pspv - 123.4567 < 0.0001);

  WL4435_TEST("DOUBLE(8, 5)", "123.45678",
              MYSQL_TYPE_DOUBLE, MYSQL_TYPE_DOUBLE,
              double, ,
              ("  - DOUBLE / double / MYSQL_TYPE_DOUBLE:\t\t %g", (double) pspv),
              pspv - 123.45678 < 0.00001);

  WL4435_TEST("DECIMAL(9, 6)", "123.456789",
              MYSQL_TYPE_NEWDECIMAL, MYSQL_TYPE_NEWDECIMAL,
              char, [255],
              ("  - DECIMAL / char[] / MYSQL_TYPE_NEWDECIMAL:\t\t '%s'", (char *) pspv),
              !strcmp(pspv, "123.456789"));

  WL4435_TEST("CHAR(32)", "REPEAT('C', 16)",
              MYSQL_TYPE_STRING, MYSQL_TYPE_STRING,
              char, [255],
              ("  - CHAR(32) / char[] / MYSQL_TYPE_STRING:\t\t '%s'", (char *) pspv),
              !strcmp(pspv, "CCCCCCCCCCCCCCCC"));

  WL4435_TEST("VARCHAR(32)", "REPEAT('V', 16)",
              MYSQL_TYPE_VAR_STRING, MYSQL_TYPE_VAR_STRING,
              char, [255],
              ("  - VARCHAR(32) / char[] / MYSQL_TYPE_VAR_STRING:\t '%s'", (char *) pspv),
              !strcmp(pspv, "VVVVVVVVVVVVVVVV"));

  WL4435_TEST("TINYTEXT", "REPEAT('t', 16)",
              MYSQL_TYPE_TINY_BLOB, MYSQL_TYPE_BLOB,
              char, [255],
              ("  - TINYTEXT / char[] / MYSQL_TYPE_TINY_BLOB:\t\t '%s'", (char *) pspv),
              !strcmp(pspv, "tttttttttttttttt"));

  WL4435_TEST("TEXT", "REPEAT('t', 16)",
              MYSQL_TYPE_BLOB, MYSQL_TYPE_BLOB,
              char, [255],
              ("  - TEXT / char[] / MYSQL_TYPE_BLOB:\t\t\t '%s'", (char *) pspv),
              !strcmp(pspv, "tttttttttttttttt"));

  WL4435_TEST("MEDIUMTEXT", "REPEAT('t', 16)",
              MYSQL_TYPE_MEDIUM_BLOB, MYSQL_TYPE_BLOB,
              char, [255],
              ("  - MEDIUMTEXT / char[] / MYSQL_TYPE_MEDIUM_BLOB:\t '%s'", (char *) pspv),
              !strcmp(pspv, "tttttttttttttttt"));

  WL4435_TEST("LONGTEXT", "REPEAT('t', 16)",
              MYSQL_TYPE_LONG_BLOB, MYSQL_TYPE_BLOB,
              char, [255],
              ("  - LONGTEXT / char[] / MYSQL_TYPE_LONG_BLOB:\t\t '%s'", (char *) pspv),
              !strcmp(pspv, "tttttttttttttttt"));

  WL4435_TEST("BINARY(32)", "REPEAT('\1', 16)",
              MYSQL_TYPE_STRING, MYSQL_TYPE_STRING,
              char, [255],
              ("  - BINARY(32) / char[] / MYSQL_TYPE_STRING:\t\t '%s'", (char *) pspv),
              memset(tmp, 1, 16) && !memcmp(tmp, pspv, 16));

  WL4435_TEST("VARBINARY(32)", "REPEAT('\1', 16)",
              MYSQL_TYPE_VAR_STRING, MYSQL_TYPE_VAR_STRING,
              char, [255],
              ("  - VARBINARY(32) / char[] / MYSQL_TYPE_VAR_STRING:\t '%s'", (char *) pspv),
              memset(tmp, 1, 16) && !memcmp(tmp, pspv, 16));

  WL4435_TEST("TINYBLOB", "REPEAT('\2', 16)",
              MYSQL_TYPE_TINY_BLOB, MYSQL_TYPE_BLOB,
              char, [255],
              ("  - TINYBLOB / char[] / MYSQL_TYPE_TINY_BLOB:\t\t '%s'", (char *) pspv),
              memset(tmp, 2, 16) && !memcmp(tmp, pspv, 16));

  WL4435_TEST("BLOB", "REPEAT('\2', 16)",
              MYSQL_TYPE_BLOB, MYSQL_TYPE_BLOB,
              char, [255],
              ("  - BLOB / char[] / MYSQL_TYPE_BLOB:\t\t\t '%s'", (char *) pspv),
              memset(tmp, 2, 16) && !memcmp(tmp, pspv, 16));

  WL4435_TEST("MEDIUMBLOB", "REPEAT('\2', 16)",
              MYSQL_TYPE_MEDIUM_BLOB, MYSQL_TYPE_BLOB,
              char, [255],
              ("  - MEDIUMBLOB / char[] / MYSQL_TYPE_MEDIUM_BLOB:\t '%s'", (char *) pspv),
              memset(tmp, 2, 16) && !memcmp(tmp, pspv, 16));

  WL4435_TEST("LONGBLOB", "REPEAT('\2', 16)",
              MYSQL_TYPE_LONG_BLOB, MYSQL_TYPE_BLOB,
              char, [255],
              ("  - LONGBLOB / char[] / MYSQL_TYPE_LONG_BLOB:\t\t '%s'", (char *) pspv),
              memset(tmp, 2, 16) && !memcmp(tmp, pspv, 16));
}