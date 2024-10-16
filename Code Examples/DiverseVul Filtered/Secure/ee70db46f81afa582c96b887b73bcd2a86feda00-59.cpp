		void CWebServer::RType_Notifications(WebEmSession & session, const request& req, Json::Value &root)
		{
			root["status"] = "OK";
			root["title"] = "Notifications";

			int ii = 0;

			//Add known notification systems
			for (const auto & ittNotifiers : m_notifications.m_notifiers)
			{
				root["notifiers"][ii]["name"] = ittNotifiers.first;
				root["notifiers"][ii]["description"] = ittNotifiers.first;
				ii++;
			}

			uint64_t idx = 0;
			if (request::findValue(&req, "idx") != "")
			{
				idx = std::strtoull(request::findValue(&req, "idx").c_str(), nullptr, 10);
			}
			std::vector<_tNotification> notifications = m_notifications.GetNotifications(idx);
			if (notifications.size() > 0)
			{
				ii = 0;
				for (const auto & itt : notifications)
				{
					root["result"][ii]["idx"] = itt.ID;
					std::string sParams = itt.Params;
					if (sParams.empty()) {
						sParams = "S";
					}
					root["result"][ii]["Params"] = sParams;
					root["result"][ii]["Priority"] = itt.Priority;
					root["result"][ii]["SendAlways"] = itt.SendAlways;
					root["result"][ii]["CustomMessage"] = itt.CustomMessage;
					root["result"][ii]["ActiveSystems"] = itt.ActiveSystems;
					ii++;
				}
			}
		}