		void CWebServer::RType_UpdateScene(WebEmSession & session, const request& req, Json::Value &root)
		{
			if (session.rights != 2)
			{
				session.reply_status = reply::forbidden;
				return; //Only admin user allowed
			}

			std::string idx = request::findValue(&req, "idx");
			std::string name = request::findValue(&req, "name");
			std::string description = request::findValue(&req, "description");
			if ((idx.empty()) || (name.empty()))
				return;
			std::string stype = request::findValue(&req, "scenetype");
			if (stype.empty())
			{
				root["status"] = "ERR";
				root["message"] = "No Scene Type specified!";
				return;
			}
			std::string tmpstr = request::findValue(&req, "protected");
			int iProtected = (tmpstr == "true") ? 1 : 0;

			std::string onaction = base64_decode(request::findValue(&req, "onaction"));
			std::string offaction = base64_decode(request::findValue(&req, "offaction"));


			root["status"] = "OK";
			root["title"] = "UpdateScene";
			m_sql.safe_query("UPDATE Scenes SET Name='%q', Description='%q', SceneType=%d, Protected=%d, OnAction='%q', OffAction='%q' WHERE (ID == '%q')",
				name.c_str(),
				description.c_str(),
				atoi(stype.c_str()),
				iProtected,
				onaction.c_str(),
				offaction.c_str(),
				idx.c_str()
			);
			uint64_t ullidx = std::strtoull(idx.c_str(), nullptr, 10);
			m_mainworker.m_eventsystem.WWWUpdateSingleState(ullidx, name, m_mainworker.m_eventsystem.REASON_SCENEGROUP);
		}