		void CWebServer::Cmd_AddLogMessage(WebEmSession & session, const request& req, Json::Value &root)
		{
			std::string smessage = request::findValue(&req, "message");
			if (smessage.empty())
				return;
			root["status"] = "OK";
			root["title"] = "AddLogMessage";

			_log.Log(LOG_STATUS, "%s", smessage.c_str());
		}