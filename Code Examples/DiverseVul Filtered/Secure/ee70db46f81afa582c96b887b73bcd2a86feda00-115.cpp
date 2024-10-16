		void CWebServer::Cmd_RenameScene(WebEmSession & session, const request& req, Json::Value &root)
		{
			if (session.rights != 2)
			{
				session.reply_status = reply::forbidden;
				return; //Only admin user allowed
			}

			std::string sidx = request::findValue(&req, "idx");
			std::string sname = request::findValue(&req, "name");
			if (
				(sidx.empty()) ||
				(sname.empty())
				)
				return;
			int idx = atoi(sidx.c_str());
			root["status"] = "OK";
			root["title"] = "RenameScene";

			m_sql.safe_query("UPDATE Scenes SET Name='%q' WHERE (ID == %d)", sname.c_str(), idx);
			uint64_t ullidx = std::strtoull(sidx.c_str(), nullptr, 10);
			m_mainworker.m_eventsystem.WWWUpdateSingleState(ullidx, sname, m_mainworker.m_eventsystem.REASON_SCENEGROUP);
		}