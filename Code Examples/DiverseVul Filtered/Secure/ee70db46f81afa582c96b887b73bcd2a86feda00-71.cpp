		void CWebServer::RemoveSession(const std::string & sessionId) {
			//_log.Log(LOG_STATUS, "SessionStore : remove...");
			if (sessionId.empty()) {
				return;
			}
			m_sql.safe_query(
				"DELETE FROM UserSessions WHERE SessionID = '%q'",
				sessionId.c_str());
		}