		void CWebServer::Cmd_SetThermostatState(WebEmSession & session, const request& req, Json::Value &root)
		{
			std::string sstate = request::findValue(&req, "state");
			std::string idx = request::findValue(&req, "idx");
			std::string name = request::findValue(&req, "name");

			if (
				(idx.empty()) ||
				(sstate.empty())
				)
				return;
			int iState = atoi(sstate.c_str());

			int urights = 3;
			bool bHaveUser = (session.username != "");
			if (bHaveUser)
			{
				int iUser = FindUser(session.username.c_str());
				if (iUser != -1)
				{
					urights = static_cast<int>(m_users[iUser].userrights);
					_log.Log(LOG_STATUS, "User: %s initiated a Thermostat State change command", m_users[iUser].Username.c_str());
				}
			}
			if (urights < 1)
				return;

			root["status"] = "OK";
			root["title"] = "Set Thermostat State";
			_log.Log(LOG_NORM, "Setting Thermostat State....");
			m_mainworker.SetThermostatState(idx, iState);
		}