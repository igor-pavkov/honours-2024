		void CWebServer::Cmd_AddUserVariable(WebEmSession & session, const request& req, Json::Value &root)
		{
			if (session.rights != 2)
			{
				session.reply_status = reply::forbidden;
				return; //Only admin user allowed
			}
			std::string variablename = request::findValue(&req, "vname");
			std::string variablevalue = request::findValue(&req, "vvalue");
			std::string variabletype = request::findValue(&req, "vtype");
			if (
				(variablename.empty()) ||
				(variabletype.empty()) ||
				((variablevalue.empty()) && (variabletype != "2"))
				)
				return;

			root["title"] = "AddUserVariable";

			std::string errorMessage;
			if (!m_sql.AddUserVariable(variablename, (const _eUsrVariableType)atoi(variabletype.c_str()), variablevalue, errorMessage))
			{
				root["status"] = "ERR";
				root["message"] = errorMessage;
			}
			else {
				root["status"] = "OK";
			}
		}