		void CWebServer::Cmd_GetTitle(WebEmSession & session, const request& req, Json::Value &root)
		{
			std::string sValue;
			root["status"] = "OK";
			root["title"] = "GetTitle";
			if (m_sql.GetPreferencesVar("Title", sValue))
				root["Title"] = sValue;
			else
				root["Title"] = "Domoticz";
		}