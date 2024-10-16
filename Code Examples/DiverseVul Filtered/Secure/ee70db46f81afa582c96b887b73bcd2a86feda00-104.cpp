		void CWebServer::RType_GetSharedUserDevices(WebEmSession & session, const request& req, Json::Value &root)
		{
			std::string idx = request::findValue(&req, "idx");
			if (idx.empty())
				return;
			root["status"] = "OK";
			root["title"] = "GetSharedUserDevices";

			std::vector<std::vector<std::string> > result;
			result = m_sql.safe_query("SELECT DeviceRowID FROM SharedDevices WHERE (SharedUserID == '%q')", idx.c_str());
			if (!result.empty())
			{
				int ii = 0;
				for (const auto & itt : result)
				{
					std::vector<std::string> sd = itt;
					root["result"][ii]["DeviceRowIdx"] = sd[0];
					ii++;
				}
			}
		}