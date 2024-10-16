		void CWebServer::Cmd_SendNotification(WebEmSession & session, const request& req, Json::Value &root)
		{
			std::string subject = request::findValue(&req, "subject");
			std::string body = request::findValue(&req, "body");
			std::string subsystem = request::findValue(&req, "subsystem");
			if (
				(subject.empty()) ||
				(body.empty())
				)
				return;
			if (subsystem.empty()) subsystem = NOTIFYALL;
			//Add to queue
			if (m_notifications.SendMessage(0, std::string(""), subsystem, subject, body, std::string(""), 1, std::string(""), false)) {
				root["status"] = "OK";
			}
			root["title"] = "SendNotification";
		}