		void CWebServer::Cmd_GetUserVariables(WebEmSession & session, const request& req, Json::Value &root)
		{
			std::vector<std::vector<std::string> > result;
			result = m_sql.safe_query("SELECT ID, Name, ValueType, Value, LastUpdate FROM UserVariables");
			int ii = 0;
			for (const auto & itt : result)
			{
				std::vector<std::string> sd = itt;
				root["result"][ii]["idx"] = sd[0];
				root["result"][ii]["Name"] = sd[1];
				root["result"][ii]["Type"] = sd[2];
				root["result"][ii]["Value"] = sd[3];
				root["result"][ii]["LastUpdate"] = sd[4];
				ii++;
			}
			root["status"] = "OK";
			root["title"] = "GetUserVariables";
		}