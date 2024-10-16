		void CWebServer::Cmd_DeleteDatePoint(WebEmSession & session, const request& req, Json::Value &root)
		{
			const std::string idx = request::findValue(&req, "idx");
			const std::string Date = request::findValue(&req, "date");
			if (
				(idx.empty()) ||
				(Date.empty())
				)
				return;
			root["status"] = "OK";
			root["title"] = "deletedatapoint";
			m_sql.DeleteDataPoint(idx.c_str(), Date);
		}