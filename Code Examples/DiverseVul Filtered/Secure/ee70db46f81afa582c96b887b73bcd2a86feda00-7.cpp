		void CWebServer::RType_DeleteScene(WebEmSession & session, const request& req, Json::Value &root)
		{
			if (session.rights != 2)
			{
				session.reply_status = reply::forbidden;
				return; //Only admin user allowed
			}

			std::string idx = request::findValue(&req, "idx");
			if (idx.empty())
				return;
			root["status"] = "OK";
			root["title"] = "DeleteScene";
			m_sql.safe_query("DELETE FROM Scenes WHERE (ID == '%q')", idx.c_str());
			m_sql.safe_query("DELETE FROM SceneDevices WHERE (SceneRowID == '%q')", idx.c_str());
			m_sql.safe_query("DELETE FROM SceneTimers WHERE (SceneRowID == '%q')", idx.c_str());
			m_sql.safe_query("DELETE FROM SceneLog WHERE (SceneRowID=='%q')", idx.c_str());
			uint64_t ullidx = std::strtoull(idx.c_str(), nullptr, 10);
			m_mainworker.m_eventsystem.RemoveSingleState(ullidx, m_mainworker.m_eventsystem.REASON_SCENEGROUP);
		}