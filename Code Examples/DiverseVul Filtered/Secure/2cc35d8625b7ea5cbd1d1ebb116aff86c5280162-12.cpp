	bool Connect()
	{
		unsigned int timeout = 1;
		connection = mysql_init(connection);
		mysql_options(connection,MYSQL_OPT_CONNECT_TIMEOUT,(char*)&timeout);
		std::string host = config->getString("host");
		std::string user = config->getString("user");
		std::string pass = config->getString("pass");
		std::string dbname = config->getString("name");
		int port = config->getInt("port");
		bool rv = mysql_real_connect(connection, host.c_str(), user.c_str(), pass.c_str(), dbname.c_str(), port, NULL, 0);
		if (!rv)
			return rv;
		std::string initquery;
		if (config->readString("initialquery", initquery))
		{
			mysql_query(connection,initquery.c_str());
		}
		return true;
	}