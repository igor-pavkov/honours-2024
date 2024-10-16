		void CWebServer::Cmd_GetAuth(WebEmSession & session, const request& req, Json::Value &root)
		{
			root["status"] = "OK";
			root["title"] = "GetAuth";
			if (session.rights != -1)
			{
				root["version"] = szAppVersion;
			}
			root["user"] = session.username;
			root["rights"] = session.rights;
		}