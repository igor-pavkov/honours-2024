		void CWebServer::Cmd_RemoveSceneCode(WebEmSession & session, const request& req, Json::Value &root)
		{
			if (session.rights != 2)
			{
				session.reply_status = reply::forbidden;
				return; //Only admin user allowed
			}

			std::string sceneidx = request::findValue(&req, "sceneidx");
			std::string idx = request::findValue(&req, "idx");
			std::string code = request::findValue(&req, "code");
			if (
				(idx.empty()) ||
				(sceneidx.empty()) ||
				(code.empty())
				)
				return;
			root["status"] = "OK";
			root["title"] = "RemoveSceneCode";

			std::vector<std::vector<std::string> > result;
			result = m_sql.safe_query("SELECT Activators, SceneType FROM Scenes WHERE (ID==%q)", sceneidx.c_str());
			if (result.empty())
				return;
			std::string Activators = result[0][0];
			int SceneType = atoi(result[0][1].c_str());
			if (!Activators.empty())
			{
				//Get Activator device names
				std::vector<std::string> arrayActivators;
				StringSplit(Activators, ";", arrayActivators);
				std::string newActivation = "";
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
					bool bFound = false;
					if (sID == idx)
					{
						if ((SceneType == 1) || (sCode.empty()))
						{
							bFound = true;
						}
						else
						{
							//Also check the code
							bFound = (sCode == code);
						}
					}
					if (!bFound)
					{
						if (!newActivation.empty())
							newActivation += ";";
						newActivation += sID;
						if ((SceneType == 0) && (!sCode.empty()))
						{
							newActivation += ":" + sCode;
						}
					}
				}
				if (Activators != newActivation)
				{
					m_sql.safe_query("UPDATE Scenes SET Activators='%q' WHERE (ID==%q)", newActivation.c_str(), sceneidx.c_str());
				}
			}
		}