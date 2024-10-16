		void CWebServer::Cmd_AddSceneCode(WebEmSession & session, const request& req, Json::Value &root)
		{
			if (session.rights != 2)
			{
				session.reply_status = reply::forbidden;
				return; //Only admin user allowed
			}

			std::string sceneidx = request::findValue(&req, "sceneidx");
			std::string idx = request::findValue(&req, "idx");
			std::string cmnd = request::findValue(&req, "cmnd");
			if (
				(sceneidx.empty()) ||
				(idx.empty()) ||
				(cmnd.empty())
				)
				return;
			root["status"] = "OK";
			root["title"] = "AddSceneCode";

			//First check if we do not already have this device as activation code
			std::vector<std::vector<std::string> > result;
			result = m_sql.safe_query("SELECT Activators, SceneType FROM Scenes WHERE (ID==%q)", sceneidx.c_str());
			if (result.empty())
				return;
			std::string Activators = result[0][0];
			unsigned char scenetype = atoi(result[0][1].c_str());

			if (!Activators.empty())
			{
				//Get Activator device names
				std::vector<std::string> arrayActivators;
				StringSplit(Activators, ";", arrayActivators);
				for (const auto & ittAct : arrayActivators)
				{
					std::string sCodeCmd = ittAct;

					std::vector<std::string> arrayCode;
					StringSplit(sCodeCmd, ":", arrayCode);

					std::string sID = arrayCode[0];
					std::string sCode = "";
					if (arrayCode.size() == 2)
					{
						sCode = arrayCode[1];
					}

					if (sID == idx)
					{
						if (scenetype == 1)
							return; //Group does not work with separate codes, so already there
						if (sCode == cmnd)
							return; //same code, already there!
					}
				}
			}
			if (!Activators.empty())
				Activators += ";";
			Activators += idx;
			if (scenetype == 0)
			{
				Activators += ":" + cmnd;
			}
			m_sql.safe_query("UPDATE Scenes SET Activators='%q' WHERE (ID==%q)", Activators.c_str(), sceneidx.c_str());
		}