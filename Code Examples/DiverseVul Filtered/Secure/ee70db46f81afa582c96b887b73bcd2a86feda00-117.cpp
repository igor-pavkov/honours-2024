		void CWebServer::Cmd_AddPlanActiveDevice(WebEmSession & session, const request& req, Json::Value &root)
		{
			if (session.rights != 2)
			{
				session.reply_status = reply::forbidden;
				return; //Only admin user allowed
			}

			std::string idx = request::findValue(&req, "idx");
			std::string sactivetype = request::findValue(&req, "activetype");
			std::string activeidx = request::findValue(&req, "activeidx");
			if (
				(idx.empty()) ||
				(sactivetype.empty()) ||
				(activeidx.empty())
				)
				return;
			root["status"] = "OK";
			root["title"] = "AddPlanActiveDevice";

			int activetype = atoi(sactivetype.c_str());

			//check if it is not already there
			std::vector<std::vector<std::string> > result;
			result = m_sql.safe_query("SELECT ID FROM DeviceToPlansMap WHERE (DeviceRowID=='%q') AND (DevSceneType==%d) AND (PlanID=='%q')",
				activeidx.c_str(), activetype, idx.c_str());
			if (result.empty())
			{
				m_sql.safe_query(
					"INSERT INTO DeviceToPlansMap (DevSceneType,DeviceRowID, PlanID) VALUES (%d,'%q','%q')",
					activetype,
					activeidx.c_str(),
					idx.c_str()
				);
			}
		}