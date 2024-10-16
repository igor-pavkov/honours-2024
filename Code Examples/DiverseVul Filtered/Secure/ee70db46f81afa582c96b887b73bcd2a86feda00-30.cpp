		void CWebServer::Cmd_UpdatePlan(WebEmSession & session, const request& req, Json::Value &root)
		{
			if (session.rights != 2)
			{
				session.reply_status = reply::forbidden;
				return; //Only admin user allowed
			}

			std::string idx = request::findValue(&req, "idx");
			if (idx.empty())
				return;
			std::string name = request::findValue(&req, "name");
			if (
				(name.empty())
				)
				return;

			root["status"] = "OK";
			root["title"] = "UpdatePlan";

			m_sql.safe_query(
				"UPDATE Plans SET Name='%q' WHERE (ID == '%q')",
				name.c_str(),
				idx.c_str()
			);
		}