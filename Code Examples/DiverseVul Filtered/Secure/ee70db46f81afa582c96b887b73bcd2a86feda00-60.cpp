		void CWebServer::RType_SceneLog(WebEmSession & session, const request& req, Json::Value &root)
		{
			uint64_t idx = 0;
			if (request::findValue(&req, "idx") != "")
			{
				idx = std::strtoull(request::findValue(&req, "idx").c_str(), nullptr, 10);
			}
			std::vector<std::vector<std::string> > result;

			root["status"] = "OK";
			root["title"] = "SceneLog";

			result = m_sql.safe_query("SELECT ROWID, nValue, Date FROM SceneLog WHERE (SceneRowID==%" PRIu64 ") ORDER BY Date DESC", idx);
			if (!result.empty())
			{
				int ii = 0;
				for (const auto & itt : result)
				{
					std::vector<std::string> sd = itt;

					int nValue = atoi(sd[1].c_str());
					root["result"][ii]["idx"] = sd[0];
					root["result"][ii]["Date"] = sd[2];
					root["result"][ii]["Data"] = (nValue == 0) ? "Off" : "On";
					ii++;
				}
			}
		}