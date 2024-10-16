		void CWebServer::Cmd_GetVersion(WebEmSession & session, const request& req, Json::Value &root)
		{
			root["status"] = "OK";
			root["title"] = "GetVersion";
			root["version"] = szAppVersion;
			root["hash"] = szAppHash;
			root["build_time"] = szAppDate;
			CdzVents* dzvents = CdzVents::GetInstance();
			root["dzvents_version"] = dzvents->GetVersion();
			root["python_version"] = szPyVersion;

			if (session.rights != 2)
			{
				//only admin users will receive the update notification
				root["UseUpdate"] = false;
				root["HaveUpdate"] = false;
			}
			else
			{
				root["UseUpdate"] = g_bUseUpdater;
				root["HaveUpdate"] = m_mainworker.IsUpdateAvailable(false);
				root["DomoticzUpdateURL"] = m_mainworker.m_szDomoticzUpdateURL;
				root["SystemName"] = m_mainworker.m_szSystemName;
				root["Revision"] = m_mainworker.m_iRevision;
			}
		}