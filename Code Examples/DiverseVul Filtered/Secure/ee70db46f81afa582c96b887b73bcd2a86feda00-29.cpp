		void CWebServer::Post_UploadCustomIcon(WebEmSession & session, const request& req, reply & rep)
		{
			Json::Value root;
			root["title"] = "UploadCustomIcon";
			root["status"] = "ERROR";
			root["error"] = "Invalid";
			//Only admin user allowed
			if (session.rights != 2)
			{
				session.reply_status = reply::forbidden;
				return; //Only admin user allowed
			}
			std::string zipfile = request::findValue(&req, "file");
			if (zipfile != "")
			{
				std::string ErrorMessage;
				bool bOK = m_sql.InsertCustomIconFromZip(zipfile, ErrorMessage);
				if (bOK)
				{
					root["status"] = "OK";
				}
				else
				{
					root["status"] = "ERROR";
					root["error"] = ErrorMessage;
				}
			}
			std::string jcallback = request::findValue(&req, "jsoncallback");
			if (jcallback.size() == 0) {
				reply::set_content(&rep, root.toStyledString());
				return;
			}
			reply::set_content(&rep, "var data=" + root.toStyledString() + '\n' + jcallback + "(data);");
		}