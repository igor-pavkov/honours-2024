		void CWebServer::Cmd_UpdateMobileDevice(WebEmSession & session, const request& req, Json::Value &root)
		{
			if (session.rights != 2)
			{
				session.reply_status = reply::forbidden;
				return; //Only admin user allowed
			}
			std::string sidx = request::findValue(&req, "idx");
			std::string enabled = request::findValue(&req, "enabled");
			std::string name = request::findValue(&req, "name");

			if (
				(sidx.empty()) ||
				(enabled.empty()) ||
				(name.empty())
				)
				return;
			uint64_t idx = std::strtoull(sidx.c_str(), nullptr, 10);

			m_sql.safe_query("UPDATE MobileDevices SET Name='%q', Active=%d WHERE (ID==%" PRIu64 ")",
				name.c_str(), (enabled == "true") ? 1 : 0, idx);

			root["status"] = "OK";
			root["title"] = "UpdateMobile";
		}