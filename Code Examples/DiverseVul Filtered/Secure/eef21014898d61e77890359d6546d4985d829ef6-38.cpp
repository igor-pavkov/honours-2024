static void test_bug1664()
{
    MYSQL_STMT *stmt;
    int        rc, int_data;
    const char *data;
    const char *str_data= "Simple string";
    MYSQL_BIND my_bind[2];
    const char *query= "INSERT INTO test_long_data(col2, col1) VALUES(?, ?)";

    myheader("test_bug1664");

    rc= mysql_query(mysql, "DROP TABLE IF EXISTS test_long_data");
    myquery(rc);

    rc= mysql_query(mysql, "CREATE TABLE test_long_data(col1 int, col2 long varchar)");
    myquery(rc);

    stmt= mysql_stmt_init(mysql);
    check_stmt(stmt);
    rc= mysql_stmt_prepare(stmt, query, strlen(query));
    check_execute(stmt, rc);

    verify_param_count(stmt, 2);

    bzero((char*) my_bind, sizeof(my_bind));

    my_bind[0].buffer_type= MYSQL_TYPE_STRING;
    my_bind[0].buffer= (void *)str_data;
    my_bind[0].buffer_length= strlen(str_data);

    my_bind[1].buffer= (void *)&int_data;
    my_bind[1].buffer_type= MYSQL_TYPE_LONG;

    rc= mysql_stmt_bind_param(stmt, my_bind);
    check_execute(stmt, rc);

    int_data= 1;

    /*
      Let us supply empty long_data. This should work and should
      not break following execution.
    */
    data= "";
    rc= mysql_stmt_send_long_data(stmt, 0, data, strlen(data));
    check_execute(stmt, rc);

    rc= mysql_stmt_execute(stmt);
    check_execute(stmt, rc);

    verify_col_data("test_long_data", "col1", "1");
    verify_col_data("test_long_data", "col2", "");

    rc= mysql_query(mysql, "DELETE FROM test_long_data");
    myquery(rc);

    /* This should pass OK */
    data= (char *)"Data";
    rc= mysql_stmt_send_long_data(stmt, 0, data, strlen(data));
    check_execute(stmt, rc);

    rc= mysql_stmt_execute(stmt);
    check_execute(stmt, rc);

    verify_col_data("test_long_data", "col1", "1");
    verify_col_data("test_long_data", "col2", "Data");

    /* clean up */
    rc= mysql_query(mysql, "DELETE FROM test_long_data");
    myquery(rc);

    /*
      Now we are changing int parameter and don't do anything
      with first parameter. Second mysql_stmt_execute() should run
      OK treating this first parameter as string parameter.
    */

    int_data= 2;
    /* execute */
    rc= mysql_stmt_execute(stmt);
    check_execute(stmt, rc);

    verify_col_data("test_long_data", "col1", "2");
    verify_col_data("test_long_data", "col2", str_data);

    /* clean up */
    rc= mysql_query(mysql, "DELETE FROM test_long_data");
    myquery(rc);

    /*
      Now we are sending other long data. It should not be
      concatened to previous.
    */

    data= (char *)"SomeOtherData";
    rc= mysql_stmt_send_long_data(stmt, 0, data, strlen(data));
    check_execute(stmt, rc);

    rc= mysql_stmt_execute(stmt);
    check_execute(stmt, rc);

    verify_col_data("test_long_data", "col1", "2");
    verify_col_data("test_long_data", "col2", "SomeOtherData");

    mysql_stmt_close(stmt);

    /* clean up */
    rc= mysql_query(mysql, "DELETE FROM test_long_data");
    myquery(rc);

    /* Now let us test how mysql_stmt_reset works. */
    stmt= mysql_stmt_init(mysql);
    check_stmt(stmt);
    rc= mysql_stmt_prepare(stmt, query, strlen(query));
    check_execute(stmt, rc);
    rc= mysql_stmt_bind_param(stmt, my_bind);
    check_execute(stmt, rc);

    data= (char *)"SomeData";
    rc= mysql_stmt_send_long_data(stmt, 0, data, strlen(data));
    check_execute(stmt, rc);

    rc= mysql_stmt_reset(stmt);
    check_execute(stmt, rc);

    rc= mysql_stmt_execute(stmt);
    check_execute(stmt, rc);

    verify_col_data("test_long_data", "col1", "2");
    verify_col_data("test_long_data", "col2", str_data);

    mysql_stmt_close(stmt);

    /* Final clean up */
    rc= mysql_query(mysql, "DROP TABLE test_long_data");
    myquery(rc);
}