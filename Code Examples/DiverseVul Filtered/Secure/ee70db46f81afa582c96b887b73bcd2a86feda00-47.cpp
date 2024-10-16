		void CWebServer::Cmd_AddMobileDevice(WebEmSession & session, const request& req, Json::Value &root)
		{
			std::string suuid = request::findValue(&req, "uuid");
			std::string ssenderid = request::findValue(&req, "senderid");
			std::string sname = request::findValue(&req, "name");
			std::string sdevtype = request::findValue(&req, "devicetype");
			std::string sactive = request::findValue(&req, "active");
			if (
				(suuid.empty()) ||
				(ssenderid.empty())
				)
				return;
			root["status"] = "OK";
			root["title"] = "AddMobileDevice";

			if (sactive.empty())
				sactive = "1";
			int iActive = (sactive == "1") ? 1 : 0;

			std::vector<std::vector<std::string> > result;
			result = m_sql.safe_query("SELECT ID, Name, DeviceType FROM MobileDevices WHERE (UUID=='%q')", suuid.c_str());
			if (result.empty())
			{
				//New
				m_sql.safe_query("INSERT INTO MobileDevices (Active,UUID,SenderID,Name,DeviceType) VALUES (%d,'%q','%q','%q','%q')",
					iActive,
					suuid.c_str(),
					ssenderid.c_str(),
					sname.c_str(),
					sdevtype.c_str());
			}
			else
			{
				//Update
				time_t now = mytime(NULL);
				struct tm ltime;
				localtime_r(&now, &ltime);
				m_sql.safe_query("UPDATE MobileDevices SET Active=%d, SenderID='%q', LastUpdate='%04d-%02d-%02d %02d:%02d:%02d' WHERE (UUID == '%q')",
					iActive,
					ssenderid.c_str(),
					ltime.tm_year + 1900, ltime.tm_mon + 1, ltime.tm_mday, ltime.tm_hour, ltime.tm_min, ltime.tm_sec,
					suuid.c_str()
				);

				std::string dname = result[0][1];
				std::string ddevtype = result[0][2];
				if (dname.empty() || ddevtype.empty())
				{
					m_sql.safe_query("UPDATE MobileDevices SET Name='%q', DeviceType='%q' WHERE (UUID == '%q')",
						sname.c_str(), sdevtype.c_str(),
						suuid.c_str()
					);
				}
			}
		}