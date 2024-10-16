		void CWebServer::CleanSessions() {
			//_log.Log(LOG_STATUS, "SessionStore : clean...");
			m_sql.safe_query(
				"DELETE FROM UserSessions WHERE ExpirationDate < datetime('now', 'localtime')");
		}