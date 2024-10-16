		void CWebServer::Cmd_AddPlan(WebEmSession & session, const request& req, Json::Value &root)
		{
			if (session.rights != 2)
			{
				session.reply_status = reply::forbidden;
				return; //Only admin user allowed
			}

			std::string name = request::findValue(&req, "name");
			root["status"] = "OK";
			root["title"] = "AddPlan";
			m_sql.safe_query(
				"INSERT INTO Plans (Name) VALUES ('%q')",
				name.c_str()
			);
			std::vector<std::vector<std::string> > result;
			result = m_sql.safe_query("SELECT MAX(ID) FROM Plans");
			if (!result.empty())
			{
				std::vector<std::string> sd = result[0];
				int ID = atoi(sd[0].c_str());

				root["idx"] = sd[0].c_str(); // OTO output the created ID for easier management on the caller side (if automated)
			}
		}