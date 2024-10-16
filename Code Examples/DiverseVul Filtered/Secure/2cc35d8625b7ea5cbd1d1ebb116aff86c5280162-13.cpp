	MySQLresult* DoBlockingQuery(const std::string& query)
	{

		/* Parse the command string and dispatch it to mysql */
		if (CheckConnection() && !mysql_real_query(connection, query.data(), query.length()))
		{
			/* Successfull query */
			MYSQL_RES* res = mysql_use_result(connection);
			unsigned long rows = mysql_affected_rows(connection);
			return new MySQLresult(res, rows);
		}
		else
		{
			/* XXX: See /usr/include/mysql/mysqld_error.h for a list of
			 * possible error numbers and error messages */
			SQLerror e(SQL_QREPLY_FAIL, ConvToStr(mysql_errno(connection)) + ": " + mysql_error(connection));
			return new MySQLresult(e);
		}
	}