		void CWebServer::Cmd_DeletePlan(WebEmSession & session, const request& req, Json::Value &root)
		{
			if (session.rights != 2)
			{
				session.reply_status = reply::forbidden;
				return; //Only admin user allowed
			}

			std::string idx = request::findValue(&req, "idx");
			if (idx.empty())
				return;
			root["status"] = "OK";
			root["title"] = "DeletePlan";
			m_sql.safe_query(
				"DELETE FROM DeviceToPlansMap WHERE (PlanID == '%q')",
				idx.c_str()
			);
			m_sql.safe_query(
				"DELETE FROM Plans WHERE (ID == '%q')",
				idx.c_str()
			);
		}