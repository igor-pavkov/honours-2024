		void CWebServer::Cmd_GetDevicesList(WebEmSession & session, const request& req, Json::Value &root)
		{
			root["status"] = "OK";
			root["title"] = "GetDevicesList";
			int ii = 0;
			std::vector<std::vector<std::string> > result;
			result = m_sql.safe_query("SELECT ID, Name FROM DeviceStatus WHERE (Used == 1) ORDER BY Name");
			if (!result.empty())
			{
				for (const auto & itt : result)
				{
					std::vector<std::string> sd = itt;
					root["result"][ii]["name"] = sd[1];
					root["result"][ii]["value"] = sd[0];
					ii++;
				}
			}
		}