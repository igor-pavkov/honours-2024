		void CWebServer::Cmd_ClearLog(WebEmSession & session, const request& req, Json::Value &root)
		{
			root["status"] = "OK";
			root["title"] = "ClearLog";
			_log.ClearLog();
		}