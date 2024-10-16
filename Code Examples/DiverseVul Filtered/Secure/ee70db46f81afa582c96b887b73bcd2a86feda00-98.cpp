		void CWebServer::RemoveUsersSessions(const std::string& username, const WebEmSession & exceptSession) {
			m_sql.safe_query("DELETE FROM UserSessions WHERE (Username=='%q') and (SessionID!='%q')", username.c_str(), exceptSession.id.c_str());
		}