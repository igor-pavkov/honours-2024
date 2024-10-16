		void CWebServer::Cmd_SetUnused(WebEmSession & session, const request& req, Json::Value &root)
		{
			if (session.rights != 2)
			{
				session.reply_status = reply::forbidden;
				return; //Only admin user allowed
			}

			std::string sidx = request::findValue(&req, "idx");
			if (sidx.empty())
				return;
			int idx = atoi(sidx.c_str());
			root["status"] = "OK";
			root["title"] = "SetUnused";
			m_sql.safe_query("UPDATE DeviceStatus SET Used=0 WHERE (ID == %d)", idx);
			if (m_sql.m_bEnableEventSystem)
				m_mainworker.m_eventsystem.RemoveSingleState(idx, m_mainworker.m_eventsystem.REASON_DEVICE);

#ifdef ENABLE_PYTHON
			// Notify plugin framework about the change
			m_mainworker.m_pluginsystem.DeviceModified(idx);
#endif
		}