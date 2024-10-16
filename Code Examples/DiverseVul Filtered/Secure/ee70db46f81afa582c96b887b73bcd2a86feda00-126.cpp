		void CWebServer::Cmd_VacuumDatabase(WebEmSession & session, const request& req, Json::Value &root)
		{
			if (session.rights != 2)
			{
				session.reply_status = reply::forbidden;
				return; //Only admin user allowed
			}
			root["status"] = "OK";
			root["title"] = "VacuumDatabase";

			m_sql.VacuumDatabase();
		}