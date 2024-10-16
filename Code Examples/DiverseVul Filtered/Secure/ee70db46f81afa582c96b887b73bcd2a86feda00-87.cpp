		void CWebServer::StoreSession(const WebEmStoredSession & session) {
			//_log.Log(LOG_STATUS, "SessionStore : store...");
			if (session.id.empty()) {
				_log.Log(LOG_ERROR, "SessionStore : cannot store session without id.");
				return;
			}

			char szExpires[30];
			struct tm ltime;
			localtime_r(&session.expires, &ltime);
			strftime(szExpires, sizeof(szExpires), "%Y-%m-%d %H:%M:%S", &ltime);

			std::string remote_host = (session.remote_host.size() <= 50) ? // IPv4 : 15, IPv6 : (39|45)
				session.remote_host : session.remote_host.substr(0, 50);

			WebEmStoredSession storedSession = GetSession(session.id);
			if (storedSession.id.empty()) {
				m_sql.safe_query(
					"INSERT INTO UserSessions (SessionID, Username, AuthToken, ExpirationDate, RemoteHost) VALUES ('%q', '%q', '%q', '%q', '%q')",
					session.id.c_str(),
					base64_encode(session.username).c_str(),
					session.auth_token.c_str(),
					szExpires,
					remote_host.c_str());
			}
			else {
				m_sql.safe_query(
					"UPDATE UserSessions set AuthToken = '%q', ExpirationDate = '%q', RemoteHost = '%q', LastUpdate = datetime('now', 'localtime') WHERE SessionID = '%q'",
					session.auth_token.c_str(),
					szExpires,
					remote_host.c_str(),
					session.id.c_str());
			}
		}