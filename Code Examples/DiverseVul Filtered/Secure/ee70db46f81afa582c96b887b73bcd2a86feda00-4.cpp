		void CWebServer::Cmd_LoginCheck(WebEmSession & session, const request& req, Json::Value &root)
		{
			std::string tmpusrname = request::findValue(&req, "username");
			std::string tmpusrpass = request::findValue(&req, "password");
			if (
				(tmpusrname.empty()) ||
				(tmpusrpass.empty())
				)
				return;

			std::string rememberme = request::findValue(&req, "rememberme");

			std::string usrname;
			std::string usrpass;
			if (request_handler::url_decode(tmpusrname, usrname))
			{
				if (request_handler::url_decode(tmpusrpass, usrpass))
				{
					usrname = base64_decode(usrname);
					int iUser = FindUser(usrname.c_str());
					if (iUser == -1) {
						// log brute force attack
						_log.Log(LOG_ERROR, "Failed login attempt from %s for user '%s' !", session.remote_host.c_str(), usrname.c_str());
						return;
					}
					if (m_users[iUser].Password != usrpass) {
						// log brute force attack
						_log.Log(LOG_ERROR, "Failed login attempt from %s for '%s' !", session.remote_host.c_str(), m_users[iUser].Username.c_str());
						return;
					}
					_log.Log(LOG_STATUS, "Login successful from %s for user '%s'", session.remote_host.c_str(), m_users[iUser].Username.c_str());
					root["status"] = "OK";
					root["version"] = szAppVersion;
					root["title"] = "logincheck";
					session.isnew = true;
					session.username = m_users[iUser].Username;
					session.rights = m_users[iUser].userrights;
					session.rememberme = (rememberme == "true");
					root["user"] = session.username;
					root["rights"] = session.rights;
				}
			}
		}