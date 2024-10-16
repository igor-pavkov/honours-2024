		void CWebServer::RType_GetTransfers(WebEmSession & session, const request& req, Json::Value &root)
		{
			root["status"] = "OK";
			root["title"] = "GetTransfers";

			uint64_t idx = 0;
			if (request::findValue(&req, "idx") != "")
			{
				idx = std::strtoull(request::findValue(&req, "idx").c_str(), nullptr, 10);
			}

			std::vector<std::vector<std::string> > result;
			result = m_sql.safe_query("SELECT Type, SubType FROM DeviceStatus WHERE (ID==%" PRIu64 ")",
				idx);
			if (!result.empty())
			{
				int dType = atoi(result[0][0].c_str());
				if (
					(dType == pTypeTEMP) ||
					(dType == pTypeTEMP_HUM)
					)
				{
					result = m_sql.safe_query(
						"SELECT ID, Name FROM DeviceStatus WHERE (Type=='%q') AND (ID!=%" PRIu64 ")",
						result[0][0].c_str(), idx);
				}
				else
				{
					result = m_sql.safe_query(
						"SELECT ID, Name FROM DeviceStatus WHERE (Type=='%q') AND (SubType=='%q') AND (ID!=%" PRIu64 ")",
						result[0][0].c_str(), result[0][1].c_str(), idx);
				}

				int ii = 0;
				for (const auto & itt : result)
				{
					std::vector<std::string> sd = itt;

					root["result"][ii]["idx"] = sd[0];
					root["result"][ii]["Name"] = sd[1];
					ii++;
				}
			}
		}