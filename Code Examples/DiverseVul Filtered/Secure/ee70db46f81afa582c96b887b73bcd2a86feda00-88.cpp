		void CWebServer::Cmd_DeleteMobileDevice(WebEmSession & session, const request& req, Json::Value &root)
		{
			if (session.rights != 2)
			{
				session.reply_status = reply::forbidden;
				return; //Only admin user allowed
			}
			std::string suuid = request::findValue(&req, "uuid");
			if (suuid.empty())
				return;
			std::vector<std::vector<std::string> > result;
			result = m_sql.safe_query("SELECT ID FROM MobileDevices WHERE (UUID=='%q')", suuid.c_str());
			if (result.empty())
				return;
			m_sql.safe_query("DELETE FROM MobileDevices WHERE (UUID == '%q')", suuid.c_str());
			root["status"] = "OK";
			root["title"] = "DeleteMobileDevice";
		}