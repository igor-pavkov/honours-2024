		void CWebServer::Cmd_GetThemes(WebEmSession & session, const request& req, Json::Value &root)
		{
			root["status"] = "OK";
			root["title"] = "GetThemes";
			m_mainworker.GetAvailableWebThemes();
			int ii = 0;
			for (const auto & itt : m_mainworker.m_webthemes)
			{
				root["result"][ii]["theme"] = itt;
				ii++;
			}
		}