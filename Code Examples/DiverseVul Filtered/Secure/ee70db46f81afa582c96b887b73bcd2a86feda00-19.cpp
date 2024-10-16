		void CWebServer::Cmd_DownloadUpdate(WebEmSession & session, const request& req, Json::Value &root)
		{
			if (!m_mainworker.StartDownloadUpdate())
				return;
			root["status"] = "OK";
			root["title"] = "DownloadUpdate";
		}