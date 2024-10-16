		void CWebServer::RType_AddScene(WebEmSession & session, const request& req, Json::Value &root)
		{
			if (session.rights != 2)
			{
				session.reply_status = reply::forbidden;
				return; //Only admin user allowed
			}

			std::string name = request::findValue(&req, "name");
			if (name.empty())
			{
				root["status"] = "ERR";
				root["message"] = "No Scene Name specified!";
				return;
			}
			std::string stype = request::findValue(&req, "scenetype");
			if (stype.empty())
			{
				root["status"] = "ERR";
				root["message"] = "No Scene Type specified!";
				return;
			}
			if (m_sql.DoesSceneByNameExits(name) == true)
			{
				root["status"] = "ERR";
				root["message"] = "A Scene with this Name already Exits!";
				return;
			}
			root["status"] = "OK";
			root["title"] = "AddScene";
			m_sql.safe_query(
				"INSERT INTO Scenes (Name,SceneType) VALUES ('%q',%d)",
				name.c_str(),
				atoi(stype.c_str())
			);
			if (m_sql.m_bEnableEventSystem)
			{
				m_mainworker.m_eventsystem.GetCurrentScenesGroups();
			}
		}