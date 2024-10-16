static void test_bug31669()
{
  int rc;
  static char buff[LARGE_BUFFER_SIZE+1];
#ifndef EMBEDDED_LIBRARY
  static char user[OLD_USERNAME_CHAR_LENGTH+1];
  static char db[NAME_CHAR_LEN+1];
  static char query[LARGE_BUFFER_SIZE*2];
#endif
  MYSQL* conn;

  DBUG_ENTER("test_bug31669");
  myheader("test_bug31669");

  conn= client_connect(0, MYSQL_PROTOCOL_TCP, 0);

  rc= mysql_change_user(conn, NULL, NULL, NULL);
  DIE_UNLESS(rc);

  rc= mysql_change_user(conn, "", "", "");
  DIE_UNLESS(rc);

  memset(buff, 'a', sizeof(buff) -  1);
  buff[sizeof(buff) -  1]= 0;

  mysql_close(conn);
  conn= client_connect(0, MYSQL_PROTOCOL_TCP, 0);

  rc= mysql_change_user(conn, buff, buff, buff);
  DIE_UNLESS(rc);

  rc = mysql_change_user(conn, opt_user, opt_password, current_db);
  DIE_UNLESS(!rc);

#ifndef EMBEDDED_LIBRARY
  memset(db, 'a', sizeof(db));
  db[NAME_CHAR_LEN]= 0;
  strxmov(query, "CREATE DATABASE IF NOT EXISTS ", db, NullS);
  rc= mysql_query(conn, query);
  myquery(rc);

  memset(user, 'b', sizeof(user));
  user[OLD_USERNAME_CHAR_LENGTH]= 0;
  memset(buff, 'c', sizeof(buff));
  buff[LARGE_BUFFER_SIZE]= 0;
  strxmov(query, "GRANT ALL PRIVILEGES ON *.* TO '", user, "'@'%' IDENTIFIED BY "
                 "'", buff, "' WITH GRANT OPTION", NullS);
  rc= mysql_query(conn, query);
  myquery(rc);

  strxmov(query, "GRANT ALL PRIVILEGES ON *.* TO '", user, "'@'localhost' IDENTIFIED BY "
                 "'", buff, "' WITH GRANT OPTION", NullS);
  rc= mysql_query(conn, query);
  myquery(rc);

  rc= mysql_query(conn, "FLUSH PRIVILEGES");
  myquery(rc);

  rc= mysql_change_user(conn, user, buff, db);
  DIE_UNLESS(!rc);

  user[OLD_USERNAME_CHAR_LENGTH-1]= 'a';
  rc= mysql_change_user(conn, user, buff, db);
  DIE_UNLESS(rc);

  user[OLD_USERNAME_CHAR_LENGTH-1]= 'b';
  buff[LARGE_BUFFER_SIZE-1]= 'd';
  rc= mysql_change_user(conn, user, buff, db);
  DIE_UNLESS(rc);

  buff[LARGE_BUFFER_SIZE-1]= 'c';
  db[NAME_CHAR_LEN-1]= 'e';
  rc= mysql_change_user(conn, user, buff, db);
  DIE_UNLESS(rc);

  mysql_close(conn);
  conn= client_connect(0, MYSQL_PROTOCOL_TCP, 0);

  db[NAME_CHAR_LEN-1]= 'a';
  rc= mysql_change_user(conn, user, buff, db);
  DIE_UNLESS(!rc);

  rc= mysql_change_user(conn, user + 1, buff + 1, db + 1);
  DIE_UNLESS(rc);

  rc = mysql_change_user(conn, opt_user, opt_password, current_db);
  DIE_UNLESS(!rc);

  strxmov(query, "DROP DATABASE ", db, NullS);
  rc= mysql_query(conn, query);
  myquery(rc);

  strxmov(query, "DELETE FROM mysql.user WHERE User='", user, "'", NullS);
  rc= mysql_query(conn, query);
  myquery(rc);
  DIE_UNLESS(mysql_affected_rows(conn) == 2);
#endif

  mysql_close(conn);

  DBUG_VOID_RETURN;
}