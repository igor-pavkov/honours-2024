		void CWebServer::Cmd_UpdateUserVariable(WebEmSession & session, const request& req, Json::Value &root)
		{
			if (session.rights != 2)
			{
				session.reply_status = reply::forbidden;
				return; //Only admin user allowed
			}

			std::string idx = request::findValue(&req, "idx");
			std::string variablename = request::findValue(&req, "vname");
			std::string variablevalue = request::findValue(&req, "vvalue");
			std::string variabletype = request::findValue(&req, "vtype");

			if (
				(variablename.empty()) ||
				(variabletype.empty()) ||
				((variablevalue.empty()) && (variabletype != "2"))
				)
				return;

			std::vector<std::vector<std::string> > result;
			if (idx.empty())
			{
				result = m_sql.safe_query("SELECT ID FROM UserVariables WHERE Name='%q'", variablename.c_str());
				if (result.empty())
					return;
				idx = result[0][0];
			}

			result = m_sql.safe_query("SELECT Name, ValueType FROM UserVariables WHERE ID='%q'", idx.c_str());
			if (result.empty())
				return;

			bool bTypeNameChanged = false;
			if (variablename != result[0][0])
				bTypeNameChanged = true; //new name
			else if (variabletype != result[0][1])
				bTypeNameChanged = true; //new type

			root["title"] = "UpdateUserVariable";

			std::string errorMessage;
			if (!m_sql.UpdateUserVariable(idx, variablename, (const _eUsrVariableType)atoi(variabletype.c_str()), variablevalue, !bTypeNameChanged, errorMessage))
			{
				root["status"] = "ERR";
				root["message"] = errorMessage;
			}
			else {
				root["status"] = "OK";
				if (bTypeNameChanged)
				{
					if (m_sql.m_bEnableEventSystem)
						m_mainworker.m_eventsystem.GetCurrentUserVariables();
				}
			}
		}