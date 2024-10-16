		void CWebServer::Cmd_EmailCameraSnapshot(WebEmSession & session, const request& req, Json::Value &root)
		{
			std::string camidx = request::findValue(&req, "camidx");
			std::string subject = request::findValue(&req, "subject");
			if (
				(camidx.empty()) ||
				(subject.empty())
				)
				return;
			//Add to queue
			m_sql.AddTaskItem(_tTaskItem::EmailCameraSnapshot(1, camidx, subject));
			root["status"] = "OK";
			root["title"] = "Email Camera Snapshot";
		}