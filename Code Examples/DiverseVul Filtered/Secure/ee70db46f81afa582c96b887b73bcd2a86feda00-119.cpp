		void CWebServer::Cmd_ChangePlanOrder(WebEmSession & session, const request& req, Json::Value &root)
		{
			std::string idx = request::findValue(&req, "idx");
			if (idx.empty())
				return;
			std::string sway = request::findValue(&req, "way");
			if (sway.empty())
				return;
			bool bGoUp = (sway == "0");

			std::string aOrder, oID, oOrder;

			std::vector<std::vector<std::string> > result;
			result = m_sql.safe_query("SELECT [Order] FROM Plans WHERE (ID=='%q')",
				idx.c_str());
			if (result.empty())
				return;
			aOrder = result[0][0];

			if (!bGoUp)
			{
				//Get next device order
				result = m_sql.safe_query("SELECT ID, [Order] FROM Plans WHERE ([Order]>'%q') ORDER BY [Order] ASC",
					aOrder.c_str());
				if (result.empty())
					return;
				oID = result[0][0];
				oOrder = result[0][1];
			}
			else
			{
				//Get previous device order
				result = m_sql.safe_query("SELECT ID, [Order] FROM Plans WHERE ([Order]<'%q') ORDER BY [Order] DESC",
					aOrder.c_str());
				if (result.empty())
					return;
				oID = result[0][0];
				oOrder = result[0][1];
			}
			//Swap them
			root["status"] = "OK";
			root["title"] = "ChangePlanOrder";

			m_sql.safe_query("UPDATE Plans SET [Order] = '%q' WHERE (ID='%q')",
				oOrder.c_str(), idx.c_str());
			m_sql.safe_query("UPDATE Plans SET [Order] = '%q' WHERE (ID='%q')",
				aOrder.c_str(), oID.c_str());
		}