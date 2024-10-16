		const WebEmStoredSession CWebServer::GetSession(const std::string & sessionId) {
			//_log.Log(LOG_STATUS, "SessionStore : get...");
			WebEmStoredSession session;

			if (sessionId.empty()) {
				_log.Log(LOG_ERROR, "SessionStore : cannot get session without id.");
			}
			else {
				std::vector<std::vector<std::string> > result;
				result = m_sql.safe_query("SELECT SessionID, Username, AuthToken, ExpirationDate FROM UserSessions WHERE SessionID = '%q'",
					sessionId.c_str());
				if (!result.empty()) {
					session.id = result[0][0].c_str();
					session.username = base64_decode(result[0][1]);
					session.auth_token = result[0][2].c_str();

					std::string sExpirationDate = result[0][3];
					time_t now = mytime(NULL);
					struct tm tExpirationDate;
					ParseSQLdatetime(session.expires, tExpirationDate, sExpirationDate);
					// RemoteHost is not used to restore the session
					// LastUpdate is not used to restore the session
				}
			}

			return session;
		}