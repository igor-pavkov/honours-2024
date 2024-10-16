		void CWebServer::Cmd_CheckForUpdate(WebEmSession & session, const request& req, Json::Value &root)
		{
			bool bHaveUser = (session.username != "");
			int urights = 3;
			if (bHaveUser)
			{
				int iUser = FindUser(session.username.c_str());
				if (iUser != -1)
					urights = static_cast<int>(m_users[iUser].userrights);
			}
			root["statuscode"] = urights;

			root["status"] = "OK";
			root["title"] = "CheckForUpdate";
			root["HaveUpdate"] = false;
			root["Revision"] = m_mainworker.m_iRevision;

			if (session.rights != 2)
			{
				session.reply_status = reply::forbidden;
				return; //Only admin users may update
			}

			bool bIsForced = (request::findValue(&req, "forced") == "true");

			if (!bIsForced)
			{
				int nValue = 0;
				m_sql.GetPreferencesVar("UseAutoUpdate", nValue);
				if (nValue != 1)
				{
					return;
				}
			}

			root["HaveUpdate"] = m_mainworker.IsUpdateAvailable(bIsForced);
			root["DomoticzUpdateURL"] = m_mainworker.m_szDomoticzUpdateURL;
			root["SystemName"] = m_mainworker.m_szSystemName;
			root["Revision"] = m_mainworker.m_iRevision;
		}