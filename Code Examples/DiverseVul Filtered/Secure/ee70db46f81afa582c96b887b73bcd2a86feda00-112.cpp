		void CWebServer::Cmd_DeleteUserVariable(WebEmSession & session, const request& req, Json::Value &root)
		{
			if (session.rights != 2)
			{
				session.reply_status = reply::forbidden;
				return; //Only admin user allowed
			}
			std::string idx = request::findValue(&req, "idx");
			if (idx.empty())
				return;

			m_sql.DeleteUserVariable(idx);
			root["status"] = "OK";
			root["title"] = "DeleteUserVariable";
		}