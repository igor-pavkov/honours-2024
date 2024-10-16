		void CWebServer::Cmd_SetSetpoint(WebEmSession & session, const request& req, Json::Value &root)
		{
			bool bHaveUser = (session.username != "");
			int iUser = -1;
			int urights = 3;
			if (bHaveUser)
			{
				iUser = FindUser(session.username.c_str());
				if (iUser != -1)
				{
					urights = static_cast<int>(m_users[iUser].userrights);
				}
			}
			if (urights < 1)
				return;

			std::string idx = request::findValue(&req, "idx");
			std::string setpoint = request::findValue(&req, "setpoint");
			if (
				(idx.empty()) ||
				(setpoint.empty())
				)
				return;
			root["status"] = "OK";
			root["title"] = "SetSetpoint";
			if (iUser != -1)
			{
				_log.Log(LOG_STATUS, "User: %s initiated a SetPoint command", m_users[iUser].Username.c_str());
			}
			m_mainworker.SetSetPoint(idx, static_cast<float>(atof(setpoint.c_str())));
		}