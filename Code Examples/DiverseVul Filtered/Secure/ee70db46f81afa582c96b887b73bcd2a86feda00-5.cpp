		void CWebServer::RType_TextLog(WebEmSession & session, const request& req, Json::Value &root)
		{
			uint64_t idx = 0;
			if (request::findValue(&req, "idx") != "")
			{
				idx = std::strtoull(request::findValue(&req, "idx").c_str(), nullptr, 10);
			}
			std::vector<std::vector<std::string> > result;

			root["status"] = "OK";
			root["title"] = "TextLog";

			result = m_sql.safe_query("SELECT ROWID, sValue, Date FROM LightingLog WHERE (DeviceRowID==%" PRIu64 ") ORDER BY Date DESC",
				idx);
			if (!result.empty())
			{
				int ii = 0;
				for (const auto & itt : result)
				{
					std::vector<std::string> sd = itt;

					root["result"][ii]["idx"] = sd[0];
					root["result"][ii]["Date"] = sd[2];
					root["result"][ii]["Data"] = sd[1];
					ii++;
				}
			}
		}