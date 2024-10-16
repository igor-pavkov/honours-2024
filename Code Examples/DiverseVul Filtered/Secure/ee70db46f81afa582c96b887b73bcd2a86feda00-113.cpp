		void CWebServer::Cmd_GetDeviceValueOptionWording(WebEmSession & session, const request& req, Json::Value &root)
		{
			if (session.rights != 2)
			{
				session.reply_status = reply::forbidden;
				return; //Only admin user allowed
			}
			std::string idx = request::findValue(&req, "idx");
			std::string pos = request::findValue(&req, "pos");
			if ((idx.empty()) || (pos.empty()))
				return;
			std::string wording;
			wording = CBasePush::DropdownOptionsValue(atoi(idx.c_str()), atoi(pos.c_str()));
			root["wording"] = wording;
			root["status"] = "OK";
			root["title"] = "GetDeviceValueOptions";
		}